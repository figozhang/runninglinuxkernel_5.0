#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>

struct my_timer {
	struct timer_list timer;
	struct vm_area_struct *gvma;
};

static void mem_timefunc(struct timer_list *t)
{
	struct my_timer *mytimer = from_timer(mytimer, t, timer);
	struct vm_area_struct *vma = mytimer->gvma;
	
	printk("%s: set vma = %p\n", __func__, vma);

	vma->vm_flags = 1;
	vma->vm_pgoff = 1;

}

int create_oops(struct vm_area_struct **p)
{
	unsigned long flags;
	struct vm_area_struct *vma = *p;

	flags = vma->vm_flags;

	printk("flags=0x%lx\n", flags);

	printk("%s: free vma %p\n", __func__, vma);

	kfree(*p);
	*p = NULL;
	printk("%s:vma %p\n", __func__, *p);
	
	return 0;
}

static struct my_timer *mytimer;

static int __init my_oops_init(void)
{
	int ret;

	mytimer = kmalloc(sizeof(*mytimer), GFP_KERNEL);
	if (!mytimer)
		return -ENOMEM;

	mytimer->gvma = kmalloc(sizeof (struct vm_area_struct), GFP_ATOMIC);
	if (!mytimer->gvma)
		return -ENOMEM;

	printk("%s, gvma=%p\n", __func__, mytimer->gvma);

	ret = create_oops(&mytimer->gvma);

	mytimer->timer.expires = jiffies + msecs_to_jiffies(10);
	timer_setup(&mytimer->timer, mem_timefunc, 0); 
	add_timer(&mytimer->timer);

	return 0;
}

static void __exit my_oops_exit(void)
{
	printk("goodbye\n");
}

module_init(my_oops_init);
module_exit(my_oops_exit);
