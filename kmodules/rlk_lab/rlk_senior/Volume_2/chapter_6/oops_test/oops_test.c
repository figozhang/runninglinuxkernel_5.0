#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static void create_oops(void)
{
	*(int *)0 = 0;
}

static int __init my_oops_init(void)
{
	printk("oops module init\n");
	create_oops();
	return 0;
}

static void __exit my_oops_exit(void)
{
	printk("goodbye\n");
}

module_init(my_oops_init);
module_exit(my_oops_exit);
