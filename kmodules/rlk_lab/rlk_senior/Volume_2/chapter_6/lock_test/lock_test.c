
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static DEFINE_SPINLOCK(hack_spinA);
static DEFINE_SPINLOCK(hack_spinB);
void hack_spinAB(void)
{
	printk("hack_lockdep: A->B\n");
	spin_lock(&hack_spinA);
	spin_lock(&hack_spinB);
}

void hack_spinBA(void)
{
	printk("hack_lockdep: B->A\n");
	spin_lock(&hack_spinB);
}

static int __init lockdep_test_init(void)
{
	printk("figo: my lockdep module init\n");
	hack_spinAB();
	hack_spinBA();
	return 0;
}

static void __exit lockdep_test_exit(void)
{
	printk("goodbye\n");
}
MODULE_LICENSE("GPL");
module_init(lockdep_test_init);
module_exit(lockdep_test_exit);
