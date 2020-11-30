#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/kallsyms.h>

asmlinkage long (*orig_sys_call)(unsigned int fd, unsigned int cmd,
		unsigned long arg);
static struct mm_struct *init_mm_p;
static void **syscall_table;
static pte_t *g_ptep;
static int syscall_nr = __NR_ioctl;

asmlinkage long my_new_ioctl(unsigned int fd, unsigned int cmd,
		unsigned long arg)
{
	pr_info("hello, i have hacked this sysall\n");

	return orig_sys_call(fd, cmd, arg);
}

static pte_t *walk_pagetable(unsigned long address)
{
	pgd_t *pgdp = NULL;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;

	pgdp = pgd_offset(init_mm_p, address);
	if (pgdp == NULL || pgd_none(*pgdp))
		return NULL;

	pudp = pud_offset(pgdp, address);
	if (!pudp || pud_none(*pudp))
		return NULL;

	pmdp = pmd_offset(pudp, address);
	if (!pmdp || pmd_none(*pmdp))
		return NULL;
	if (pmd_val(*pmdp) & PMD_TYPE_SECT)
		return (pte_t *)pmdp;

	ptep = pte_offset_kernel(pmdp, address);
	if ((ptep == NULL) || pte_none(*ptep))
		return NULL;

	return ptep;
}

static int __init hack_syscall_init(void)
{
	pte_t *p_pte;
	pte_t pte;
	pte_t pte_new;

	syscall_table = (void **) kallsyms_lookup_name("sys_call_table");
	if (!syscall_table) {
		pr_err("ERROR: Cannot find the system call table address.\n");
		return -EFAULT;
	}
	pr_info("Found the sys_call_table at %16lx.\n",
			(unsigned long) syscall_table);

	init_mm_p = (struct mm_struct *)kallsyms_lookup_name("init_mm");
	if (!init_mm_p) {
		pr_err("ERROR: Cannot find init_mm\n");
		return -EFAULT;
	}

	p_pte  = walk_pagetable((unsigned long)syscall_table);
	if (!p_pte)
		return -EFAULT;

	g_ptep = p_pte;
	pte = *p_pte;
	pr_info("walk_pagetable get pte=%llx", pte_val(pte));

	pte_new = pte_mkyoung(pte);
	pte_new = pte_mkwrite(pte_new);

	pr_info("mkwrite pte=%llx", pte_val(pte_new));
	pr_info("replace system call ...\n");

	set_pte_at(init_mm_p, (unsigned long)syscall_table, p_pte, pte_new);

	pr_info("before change sys_call_table[%d] at %16lx.\n",
			syscall_nr, (unsigned long)orig_sys_call);

	orig_sys_call = syscall_table[syscall_nr];
	syscall_table[syscall_nr] = my_new_ioctl;

	pr_info("after change sys_call_table[%d] at %16lx.\n",
			syscall_nr, (unsigned long)syscall_table[syscall_nr]);

	return 0;
}

static void __exit hack_syscall_exit(void)
{
	pr_info("syscall_release\n");
}
module_init(hack_syscall_init);
module_exit(hack_syscall_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Benshushu <runninglinuxkernel@126.com>");
MODULE_DESCRIPTION("Replace syscall dynamically");
