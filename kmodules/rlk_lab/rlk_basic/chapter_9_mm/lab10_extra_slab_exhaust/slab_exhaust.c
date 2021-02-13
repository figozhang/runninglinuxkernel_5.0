#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>

static char *kbuf;
static int size = 20;
static struct kmem_cache *my_cache;
module_param(size, int, 0644);

static int __init my_init(void)
{
	unsigned long i = 0;

	/* allocate a memory cache object */
	while (1) {
		kbuf = kmalloc(30, GFP_KERNEL);
		if (!kbuf) {
			pr_err(" failed to kmalloc buffer\n");
			(void)kmem_cache_destroy(my_cache);
			return -1;
		}
		i++;
	}
	pr_info("allocate %ld objects done\n\n", i);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Ben ShuShu");
