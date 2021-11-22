#include <linux/vfio.h>
#include <linux/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>

#include <time.h>

#define VFIO_BASE_PATH "/dev/vfio/"
#define VFIO_CONTAINER_PATH  "/dev/vfio/vfio"

#define VFIO_DMA_MAP_FLAG_NOEXEC (1 << 2)

struct vfio_dev_spec {
	char *name;
	char *bus;
	int iommu_group;

	struct vfio_device_info vfio_device_info;
	int device_fd;

	struct vfio_region_info *regions;
	struct vfio_irq_info *irqs;
};

struct vfio_info {
	struct vfio_group_status group_status;
	struct vfio_iommu_type1_info iommu_info;
	struct vfio_dev_spec dev;

	int container;
	int group;
};

void free_vfio_dev(struct vfio_dev_spec *dev)
{
	if (dev->regions) {
		free(dev->regions);
	}

	if (dev->irqs) {
		free(dev->irqs);
	}
}

void init_vfio_info(struct vfio_info *info)
{
	info->group_status.argsz = sizeof(struct vfio_group_status);
	info->iommu_info.argsz = sizeof(struct vfio_iommu_type1_info);
}

void init_vfio_dev_spec(struct vfio_dev_spec *dev)
{
	dev->regions = NULL;
	dev->irqs = NULL;
}

int is_group_viable(struct vfio_info *info)
{
	int ret;

	ret = ioctl(info->group, VFIO_GROUP_GET_STATUS, &info->group_status);
	if (ret)
		return ret;

	return info->group_status.flags & VFIO_GROUP_FLAGS_VIABLE;
}

int check_vfio_version(struct vfio_info *info)
{
	return ioctl(info->container, VFIO_GET_API_VERSION) != VFIO_API_VERSION;
}

int check_iommu_extension(struct vfio_info *info)
{
	return !ioctl(info->container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU);
}

int set_group_to_container(struct vfio_info *info)
{
	return ioctl(info->group, VFIO_GROUP_SET_CONTAINER, &info->container);
}

int unset_group_to_container(struct vfio_info *info)
{
	return ioctl(info->group, VFIO_GROUP_UNSET_CONTAINER, &info->container);
}

int set_iommu_type(struct vfio_info *info)
{
	return ioctl(info->container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
}

int dma_do_map(struct vfio_info *info, struct vfio_iommu_type1_dma_map *map,
						    uint64_t iova, int size)
{
	map->argsz = sizeof(*map);
	map->vaddr = (uintptr_t)mmap(0, size, PROT_READ | PROT_WRITE,
			     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	map->iova = iova;
	map->flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE;
	map->size = size;

	//memset(map->vaddr, 0x55, size);

	return ioctl(info->container, VFIO_IOMMU_MAP_DMA, map);
}

int dma_do_unmap(struct vfio_info *info, struct vfio_iommu_type1_dma_map *map)
{
	int ret;
	struct vfio_iommu_type1_dma_unmap unmap;

	unmap.argsz = sizeof(struct vfio_iommu_type1_dma_unmap);
	unmap.iova = map->iova;
	unmap.size = map->size;
	unmap.flags = map->flags;

	ret = ioctl(info->container, VFIO_IOMMU_UNMAP_DMA, &unmap);

	munmap((void *)map->vaddr, map->size);

	return ret;
}

void get_vfio_device_info(int dev_fd, struct vfio_device_info *info)
{
	info->argsz = sizeof(*info);
	ioctl(dev_fd, VFIO_DEVICE_GET_INFO, info);
}

void populate_device_regions(struct vfio_dev_spec *dev)
{
	int i;
	int num_regs = dev->vfio_device_info.num_regions;

	dev->regions = malloc(num_regs * sizeof(struct vfio_region_info));

	for (i = 0; i < num_regs; i++) {
		struct vfio_region_info *reg = &dev->regions[i];

		reg->argsz = sizeof(*reg);
		reg->index = i;

		ioctl(dev->device_fd, VFIO_DEVICE_GET_REGION_INFO, reg);
	}
}

void populate_device_irqs(struct vfio_dev_spec *dev)
{
	int i;
	int num_irqs = dev->vfio_device_info.num_irqs;

	dev->irqs = malloc(num_irqs * sizeof(struct vfio_irq_info));

	for (i = 0; i < num_irqs; i++) {
		struct vfio_irq_info *irq = &dev->irqs[i];

		irq->argsz = sizeof(*irq);
		irq->index = i;

		ioctl(dev->device_fd, VFIO_DEVICE_GET_IRQ_INFO, irq);
	}
}

int main(int argc, const char **argv)
{
	int i;
	struct vfio_info dev_vfio_info;
	struct vfio_iommu_type1_dma_map dma_map;
	struct vfio_dev_spec dev;

	init_vfio_info(&dev_vfio_info);
	init_vfio_dev_spec(&dev);

	/* Create a new container */
	dev_vfio_info.container = open(VFIO_CONTAINER_PATH, O_RDWR);

	if (check_vfio_version(&dev_vfio_info)
		|| check_iommu_extension(&dev_vfio_info)) {
		printf("IOMMU Type1 not supported or unknown API\n");

		goto error;
	}

	/* Open the group */
	dev_vfio_info.group = open("/dev/vfio/0", O_RDWR);
	if (!is_group_viable(&dev_vfio_info)) {
		printf("the group is not viable\n");

		goto error;
	}

	if (set_group_to_container(&dev_vfio_info)
			|| set_iommu_type(&dev_vfio_info)) {
		printf("something went wrong\n");

		goto error;
	}

	/* try to map 1MB of memory to the device */
	printf("dma-mapping some memory to the device\n");
	if (dma_do_map(&dev_vfio_info, &dma_map, 0, 4096)) {
		printf("error while dma-mapping\n");
	} else {
		printf("dma-map successful\n");
	}

	/* use 00:01:0 for example */
	dev.device_fd = ioctl(dev_vfio_info.group, VFIO_GROUP_GET_DEVICE_FD, "0000:00:01.0");
	if (dev.device_fd < 0) {
		printf("unable to get device fd\n");
		goto error;
	}

	get_vfio_device_info(dev.device_fd, &dev.vfio_device_info);
	populate_device_regions(&dev);

	printf("\nNum regions: %d\n", dev.vfio_device_info.num_regions);

	for (i = 0; i < dev.vfio_device_info.num_regions; i++) {
		struct vfio_region_info *reg = &dev.regions[i];
		uint32_t *mem;


		mem = (uint32_t *)mmap(NULL, reg->size, PROT_READ | PROT_WRITE,
					      MAP_SHARED, dev.device_fd, reg->offset);
		if (mem == MAP_FAILED)
			continue;
		if (mem != MAP_FAILED) {
			printf(" Successful MMAP to address %p\n", mem);
			printf("    region #%d:\n", reg->index);
			printf("        size: %llu\n", reg->size);
			printf("        offset: 0x%llx\n", reg->offset);
			printf("        flags: 0x%llx\n", reg->offset);
		}

		/* unmap */
		if (munmap(mem, reg->size)) {
			printf("error while unmapping region %d\n", i);
		}
	}

	dma_do_unmap(&dev_vfio_info, &dma_map);

	free_vfio_dev(&dev);

	unset_group_to_container(&dev_vfio_info);
	close(dev_vfio_info.group);
	close(dev_vfio_info.container);

	return 0;

error:
	exit(1);
}
