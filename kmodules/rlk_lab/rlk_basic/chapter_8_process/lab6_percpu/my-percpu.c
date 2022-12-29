#include <linux/module.h>
#include <linux/init.h>
#include <linux/percpu.h>
#include <linux/cpumask.h>

static DEFINE_PER_CPU(long, cpuvar) = 10;
static long __percpu *cpualloc;

static int __init my_init(void)
{
	int cpu;
	int cur_cpu = raw_smp_processor_id();

	pr_info("current cpu %d\n", cur_cpu);

	/* print the init value of cpuvar*/
	for_each_possible_cpu(cpu){
		pr_info("init: cpuvar on cpu%d  = %ld\n", cpu, per_cpu(cpuvar, cpu));
	}

	/* set cpu0 value */
	per_cpu(cpuvar, 0) = 15;
	per_cpu(cpuvar, 1) = 20;

	for_each_possible_cpu(cpu){
		pr_info("after motify: cpuvar on cpu%d  = %ld\n", cpu, per_cpu(cpuvar, cpu));
	}

	__this_cpu_write(cpuvar, 22);
	pr_info("cpu%d has motify to %ld\n", cur_cpu, get_cpu_var(cpuvar));
	put_cpu_var(cpuvar);

	/* alloc a percpu value */
	cpualloc = alloc_percpu(long);

	/* set all cpu for this value */
	for_each_possible_cpu(cpu){
		*per_cpu_ptr(cpualloc, cpu) = 100;
		pr_info("init: cpu:%d cpualloc = %ld\n",
				cpu, *per_cpu_ptr(cpualloc, cpu));
	}

	return 0;
}

static void __exit my_exit(void)
{
	int cpu;
	pr_info("exit module...\n");

	for_each_possible_cpu(cpu) {
		pr_info("cpuvar cpu%d = %ld\n", cpu, per_cpu(cpuvar, cpu));
		pr_info("exit: cpualloc%d = %ld\n", cpu, *per_cpu_ptr(cpualloc, cpu));
	}

	free_percpu(cpualloc);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Ben ShuShu");
MODULE_LICENSE("GPL v2");
