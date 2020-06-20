#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/sched/mm.h>

#define DEMO_NAME "my_demo_dev"

struct mydev_priv {
	struct device *dev;
	struct miscdevice *miscdev;
	struct mutex lock;
	char *name;
};

struct mydev_work {
	struct mutex *lock;
	char *name;
	struct mm_struct *mm;
	long counter;
	struct mydev_priv *mydev;
};

static struct mydev_priv *g_mydev;

/*virtual FIFO device's buffer*/
static char *device_buffer;
#define MAX_DEVICE_BUFFER_SIZE (10 * PAGE_SIZE)

#define MYDEV_CMD_GET_BUFSIZE 1	/* defines our IOCTL cmd */

static int demodrv_open(struct inode *inode, struct file *file)
{
	struct mydev_priv *priv = g_mydev;
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);

	struct task_struct *task = current;
	struct mm_struct *mm = task->mm;

	down_read(&mm->mmap_sem);

	printk("%s: major=%d, minor=%d, name=%s\n", __func__, major, minor, priv->name);

	return 0;
}

static int demodrv_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int nbytes = 
		simple_read_from_buffer(buf, count, ppos, device_buffer, MAX_DEVICE_BUFFER_SIZE);

	printk("%s: read nbytes=%d done at pos=%d\n",
		 __func__, nbytes, (int)*ppos);

	return nbytes;
}

static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int nbytes = 
		simple_write_to_buffer(device_buffer, MAX_DEVICE_BUFFER_SIZE, ppos, buf, count);

	printk("%s: write nbytes=%d done at pos=%d\n",
		 __func__, nbytes, (int)*ppos);

	return nbytes;
}

static int 
demodrv_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long pfn;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long len = vma->vm_end - vma->vm_start;

	if (offset >= MAX_DEVICE_BUFFER_SIZE)
		return -EINVAL;
	if (len > (MAX_DEVICE_BUFFER_SIZE - offset))
		return -EINVAL;

	printk("%s: mapping %ld bytes of device buffer at offset %ld\n",
		 __func__, len, offset);

	/*    pfn = page_to_pfn (virt_to_page (ramdisk + offset)); */
	pfn = virt_to_phys(device_buffer + offset) >> PAGE_SHIFT;

	if (remap_pfn_range(vma, vma->vm_start, pfn, len, vma->vm_page_prot))
		return -EAGAIN;

	return 0;
}

static long
demodrv_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct mydev_priv *priv = g_mydev;
	unsigned long tbs = MAX_DEVICE_BUFFER_SIZE;
	void __user *ioargp = (void __user *)arg;

	switch (cmd) {
	default:
		return -EINVAL;

	case MYDEV_CMD_GET_BUFSIZE:
		mutex_lock(&priv->lock);
		if (copy_to_user(ioargp, &tbs, sizeof(tbs)))
			return -EFAULT;
		return 0;
	}
}

static const struct file_operations demodrv_fops = {
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write,
	.mmap = demodrv_mmap,
	.unlocked_ioctl = demodrv_unlocked_ioctl,
};

static struct miscdevice miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEMO_NAME,
	.fops = &demodrv_fops,
};

int do_work(struct mydev_work *work)
{
	struct mm_struct *mm;
	char buf[100];

	mm = get_task_mm(current);

	mutex_lock(work->lock);
	mdelay(1000);
	memset(buf, 0x55, sizeof(buf));
	mutex_unlock(work->lock);
	
	return 0;
}

int lockdep_thread1(void *p)
{
	struct mydev_priv *priv;
	struct mydev_work work = {0,};

	priv = (struct mydev_priv *)p;

	work.lock = &priv->lock;
	work.name = "thread1";
	work.counter++;
	work.mydev = priv;
	work.mm = get_task_mm(current);

	set_freezable();
	set_user_nice(current, 0);

	while (!kthread_should_stop()) {
		do_work(&work);
	}
	return 0;
}

static int lockdep_thread2(void *p)
{
	struct mydev_priv *priv;
	struct mydev_work work = {0,};

	priv = (struct mydev_priv *)p;

	work.lock = &priv->lock;
	work.counter++;
	work.mydev = priv;
	work.mm = get_task_mm(current);
	work.name = "thread2";

	set_freezable();
	set_user_nice(current, 0);

	while (!kthread_should_stop()) {
		do_work(&work);
	}
	return 0;
}

static struct task_struct *lock_thread1;
static struct task_struct *lock_thread2;

static int __init simple_char_init(void)
{
	int ret;
	struct mydev_priv *mydev;

	mydev = kmalloc(sizeof (*mydev), GFP_KERNEL);
	if (!mydev)
		return -ENOMEM;

	mydev->name = DEMO_NAME;

	device_buffer = kmalloc(MAX_DEVICE_BUFFER_SIZE, GFP_KERNEL);
	if (!device_buffer)
		return -ENOMEM;

	ret = misc_register(&miscdev);
	if (ret) {
		printk("failed register misc device\n");
		kfree(device_buffer);
		return ret;
	}

	mutex_init(&mydev->lock);

	mydev->dev = miscdev.this_device;
	mydev->miscdev = &miscdev;

	lock_thread1 = kthread_run(lockdep_thread1, mydev, "lock_test1");
	lock_thread2 = kthread_run(lockdep_thread2, mydev, "lock_test2");

	dev_set_drvdata(mydev->dev, mydev);

	g_mydev = mydev;

	printk("succeeded register char device: %s\n", DEMO_NAME);

	return 0;
}

static void __exit simple_char_exit(void)
{
	struct mydev_priv *priv = g_mydev;
	printk("removing device\n");

	kfree(device_buffer);
	misc_deregister(priv->miscdev);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Benshushu");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("simpe character device");
