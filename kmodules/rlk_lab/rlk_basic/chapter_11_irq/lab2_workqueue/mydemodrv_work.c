#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DEMO_NAME "mydemo_dev"
#define MYDEMO_FIFO_SIZE 64

static dev_t dev;
static struct cdev *demo_cdev;
static struct class *mydemo_class;

struct mydemo_device {
	char name[64];
	struct device *dev;
        wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;	
	struct kfifo mydemo_fifo;
	struct fasync_struct *fasync;
	struct mutex lock;
};

struct mydemo_private_data {
	struct mydemo_device *device;
	char name[64];	
	struct tasklet_struct tasklet;
	struct work_struct my_work;
};

#define MYDEMO_MAX_DEVICES  8
static struct mydemo_device *mydemo_device[MYDEMO_MAX_DEVICES]; 

static void do_tasklet(unsigned long data)
{
	struct mydemo_device *device = (struct mydemo_device *)data;

	dev_info(device->dev, "%s: trigger a tasklet\n", __func__);
}

static void do_work(struct work_struct *work)
{
	struct mydemo_private_data *data;
	struct mydemo_device *device;

	data = container_of(work, struct mydemo_private_data, my_work);	

	device = data->device;

	dev_info(device->dev, "%s: trigger a work\n", __func__);
}

static int demodrv_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct mydemo_private_data *data;
	struct mydemo_device *device = mydemo_device[minor];

	dev_info(device->dev, "%s: major=%d, minor=%d, device=%s\n", __func__, 
			MAJOR(inode->i_rdev), MINOR(inode->i_rdev), device->name);

	data = kzalloc(sizeof(struct mydemo_private_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	sprintf(data->name, "private_data_%d", minor);
	tasklet_init(&data->tasklet, do_tasklet, (unsigned long)device);
	INIT_WORK(&data->my_work, do_work);

	data->device = device;
	file->private_data = data;

	return 0;
}

static int demodrv_release(struct inode *inode, struct file *file)
{
	struct mydemo_private_data *data = file->private_data;

	tasklet_kill(&data->tasklet);
	kfree(data);
    
	return 0;
}

static ssize_t
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct mydemo_private_data *data = file->private_data;
	struct mydemo_device *device = data->device;
	int actual_readed;
	int ret;


	if (kfifo_is_empty(&device->mydemo_fifo)) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		dev_info(device->dev, "%s:%s pid=%d, going to sleep, %s\n", __func__, device->name, current->pid, data->name);
		ret = wait_event_interruptible(device->read_queue,
					!kfifo_is_empty(&device->mydemo_fifo));
		if (ret)
			return ret;
	}

	mutex_lock(&device->lock);
	ret = kfifo_to_user(&device->mydemo_fifo, buf, count, &actual_readed);
	if (ret)
		return -EIO;
	mutex_unlock(&device->lock);

	if (!kfifo_is_full(&device->mydemo_fifo)){
		wake_up_interruptible(&device->write_queue);
		kill_fasync(&device->fasync, SIGIO, POLL_OUT);
	}
	
	dev_info(device->dev, "%s:%s, pid=%d, actual_readed=%d, pos=%lld\n",__func__,
			device->name, current->pid, actual_readed, *ppos);
	return actual_readed;
}

static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct mydemo_private_data *data = file->private_data;
	struct mydemo_device *device = data->device;

	unsigned int actual_write;
	int ret;

	if (kfifo_is_full(&device->mydemo_fifo)){
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		dev_info(device->dev, "%s:%s pid=%d, going to sleep\n", __func__, device->name, current->pid);
		ret = wait_event_interruptible(device->write_queue,
				!kfifo_is_full(&device->mydemo_fifo));
		if (ret)
			return ret;
	}

	mutex_lock(&device->lock);

	ret = kfifo_from_user(&device->mydemo_fifo, buf, count, &actual_write);
	if (ret)
		return -EIO;
	schedule_work(&data->my_work);
	mutex_unlock(&device->lock);

	if (!kfifo_is_empty(&device->mydemo_fifo)) {
		wake_up_interruptible(&device->read_queue);
		kill_fasync(&device->fasync, SIGIO, POLL_IN);
		printk("%s kill fasync\n", __func__);
	}

	dev_info(device->dev, "%s:%s pid=%d, actual_write =%d, ppos=%lld, ret=%d\n", __func__,
			device->name, current->pid, actual_write, *ppos, ret);

	return actual_write;
}

static unsigned int demodrv_poll(struct file *file, poll_table *wait)
{
	int mask = 0;
	struct mydemo_private_data *data = file->private_data;
	struct mydemo_device *device = data->device;

	mutex_lock(&device->lock);

	poll_wait(file, &device->read_queue, wait);
        poll_wait(file, &device->write_queue, wait);

	if (!kfifo_is_empty(&device->mydemo_fifo))
		mask |= POLLIN | POLLRDNORM;
	if (!kfifo_is_full(&device->mydemo_fifo))
		mask |= POLLOUT | POLLWRNORM;

	mutex_unlock(&device->lock);
	
	return mask;
}

static int demodrv_fasync(int fd, struct file *file, int on)
{
	struct mydemo_private_data *data = file->private_data;
	struct mydemo_device *device = data->device;
	int ret;

	mutex_lock(&device->lock);

	dev_info(device->dev, "%s send SIGIO\n", __func__);
	ret = fasync_helper(fd, file, on, &device->fasync);
	mutex_unlock(&device->lock);

	return ret;
}

static const struct file_operations demodrv_fops = {
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write,
        .poll = demodrv_poll,
	.fasync = demodrv_fasync,
};

static int __init simple_char_init(void)
{
	int ret;
	int i;
	struct mydemo_device *device;
	
	ret = alloc_chrdev_region(&dev, 0, MYDEMO_MAX_DEVICES, DEMO_NAME);
	if (ret) {
		printk("failed to allocate char device region");
		return ret;
	}

	demo_cdev = cdev_alloc();
	if (!demo_cdev) {
		printk("cdev_alloc failed\n");
		goto unregister_chrdev;
	}

	cdev_init(demo_cdev, &demodrv_fops);
	
	ret = cdev_add(demo_cdev, dev, MYDEMO_MAX_DEVICES);
	if (ret) {
		printk("cdev_add failed\n");
		goto cdev_fail;
	}
	
	mydemo_class = class_create(THIS_MODULE, "my_class");

	for (i = 0; i < MYDEMO_MAX_DEVICES; i++) {
		device = kzalloc(sizeof(struct mydemo_device), GFP_KERNEL);
		if (!device) {
			ret = -ENOMEM;
			goto free_device;
		}

		sprintf(device->name, "%s%d", DEMO_NAME, i);
		mutex_init(&device->lock);

		device->dev = device_create(mydemo_class, NULL, MKDEV(dev, i), NULL, "mydemo:%d:%d", MAJOR(dev), i);
		dev_info(device->dev, "create device: %d:%d\n", MAJOR(dev), MINOR(i));
		mydemo_device[i] = device;
		init_waitqueue_head(&device->read_queue);
		init_waitqueue_head(&device->write_queue);

		ret = kfifo_alloc(&device->mydemo_fifo,
				MYDEMO_FIFO_SIZE,
				GFP_KERNEL);
		if (ret) {
			ret = -ENOMEM;
			goto free_kfifo;
		}

		printk("mydemo_fifo=%p\n", &device->mydemo_fifo);

	}

	printk("succeeded register char device: %s\n", DEMO_NAME);

	return 0;

free_kfifo:
	for (i =0; i < MYDEMO_MAX_DEVICES; i++)
		if (&device->mydemo_fifo)
			 kfifo_free(&device->mydemo_fifo);
free_device:
	for (i =0; i < MYDEMO_MAX_DEVICES; i++)
		if (mydemo_device[i])
			kfree(mydemo_device[i]);
cdev_fail:
	cdev_del(demo_cdev);
unregister_chrdev:
	unregister_chrdev_region(dev, MYDEMO_MAX_DEVICES);
	return ret;
}

static void __exit simple_char_exit(void)
{
	int i;
	printk("removing device\n");

	if (demo_cdev)
		cdev_del(demo_cdev);

	unregister_chrdev_region(dev, MYDEMO_MAX_DEVICES);

	for (i =0; i < MYDEMO_MAX_DEVICES; i++) {
		if (mydemo_device[i]) {
			device_destroy(mydemo_class, MKDEV(dev, i));
			kfree(mydemo_device[i]);	

		}
	}
	class_destroy(mydemo_class);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Benshushu");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simpe character device");
