#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/genhd.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/version.h>

#define MY_DEVICE_NAME "myramdisk"

static int mybdrv_ma_no, diskmb = 10, disk_size;
static char *ramdisk;
static struct gendisk *my_gd;
static unsigned short sector_size = 512;
static struct request_queue *my_request_queue;

module_param_named(size, diskmb, int, 0);

static int do_bvec(struct page *page, unsigned int len, 
						unsigned int off, unsigned int op, 
						sector_t sector)
{
	void *mem;
	char *ptr;

	ptr = ramdisk + (sector << SECTOR_SHIFT);

	mem = kmap_atomic(page);
	if (op_is_write(op)) {
		pr_info("writing at sector %d, %u sectors\n",
			(unsigned int)sector, (unsigned int)(len >> SECTOR_SHIFT));
		flush_dcache_page(page);
		memcpy(ptr, mem+off, len);
	} else {
		pr_info("reading at sector %d, %u sectors\n",
			(unsigned int)sector, (unsigned int)(len >> SECTOR_SHIFT));
		flush_dcache_page(page);
		memcpy(mem+off, ptr, len);
	}
	kunmap_atomic(mem);

	return 0;
}

static blk_qc_t my_request(struct request_queue *q, struct bio *bio)
{
	struct bio_vec bvec;
	sector_t sector;
	struct bvec_iter iter;
	
	sector = bio->bi_iter.bi_sector;
	if (bio_end_sector(bio) > get_capacity(bio->bi_disk))
		goto io_error;

	bio_for_each_segment(bvec, bio, iter) {
		unsigned int len = bvec.bv_len;
		int err;

		err = do_bvec(bvec.bv_page, len, bvec.bv_offset,
				bio_op(bio), sector);
		if (err)
			goto io_error;
		sector += len >> SECTOR_SHIFT;
	}

	bio_endio(bio);
	return BLK_QC_T_NONE;
io_error:
	bio_io_error(bio);
	return BLK_QC_T_NONE;
}

static int my_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	long size;
	
	size = disk_size;
	size &= ~0x3f;
	geo->cylinders = size>>6;
	geo->heads = 2;
	geo->sectors = 16;
	geo->start = 4;

	return 0;
}

static const struct block_device_operations mybdrv_fops = {
	.owner = THIS_MODULE,
	.getgeo = my_getgeo,
};

static int __init my_init(void)
{
	disk_size = diskmb * 1024 * 1024;

	ramdisk = vmalloc(disk_size);
	if (!ramdisk)
		return -ENOMEM;

	my_request_queue = blk_alloc_queue(GFP_KERNEL);
	if (!my_request_queue) {
		vfree(ramdisk);
		return -ENOMEM;
	}
	blk_queue_make_request(my_request_queue, my_request);
	blk_queue_max_hw_sectors(my_request_queue, UINT_MAX);
	blk_queue_physical_block_size(my_request_queue, sector_size);

	mybdrv_ma_no = register_blkdev(0, MY_DEVICE_NAME);
	if (mybdrv_ma_no < 0) {
		pr_err("Failed registering mybdrv, returned %d\n",
		       mybdrv_ma_no);
		blk_cleanup_queue(my_request_queue);
		vfree(ramdisk);
		return mybdrv_ma_no;
	}

	my_gd = alloc_disk(16);
	if (!my_gd) {
		unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
		blk_cleanup_queue(my_request_queue);
		vfree(ramdisk);
		return -ENOMEM;
	}

	my_gd->major = mybdrv_ma_no;
	my_gd->first_minor = 0;
	my_gd->fops = &mybdrv_fops;
	strcpy(my_gd->disk_name, MY_DEVICE_NAME);
	my_gd->queue = my_request_queue;
	my_gd->flags = GENHD_FL_EXT_DEVT;
	set_capacity(my_gd, disk_size / sector_size);
	add_disk(my_gd);

	pr_info("device successfully registered, Major No. = %d\n",
		mybdrv_ma_no);
	pr_info("Capacity of ram disk is: %d MB\n", diskmb);

	return 0;
}

static void __exit my_exit(void)
{
	del_gendisk(my_gd);
	put_disk(my_gd);
	unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
	pr_info("module successfully unloaded, Major No. = %d\n", mybdrv_ma_no);
	blk_cleanup_queue(my_request_queue);
	vfree(ramdisk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Benshushu");
MODULE_LICENSE("GPL v2");

