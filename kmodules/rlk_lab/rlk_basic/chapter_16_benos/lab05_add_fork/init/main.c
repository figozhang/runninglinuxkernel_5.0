#include <uart.h>
#include <asm/sysregs.h>
#include <printk.h>
#include <irq.h>
#include <asm/timer.h>
#include <sched.h>
#include <memory.h>

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];

extern union task_union init_task_union;

static void print_mem(void)
{
	printk("BenOS image layout:\n");
	printk("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
			(u64)_text_boot, (u64)_etext_boot,
			(u32)(_etext_boot - _text_boot));
	printk("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
			(u64)_text, (u64)_etext,
			(u32)(_etext - _text));
	printk("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
			(u64)_rodata, (u64)_erodata,
			(u32)(_erodata - _rodata));
	printk("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
			(u64)_data, (u64)_edata,
			(u32)(_edata - _data));
	printk("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
			(u64)_bss, (u64)_ebss,
			(u32)(_ebss - _bss));
}

static void delay(int n)
{
	while (n--)
		;
}

void kernel_thread(void)
{
	while (1) {
		delay(10000000);
		printk("%s: %s\n", __func__, "12345");
	}
}

register unsigned long current_stack_pointer asm ("sp");

void kernel_main(void)
{
	int el;

	uart_init();
	uart_send_string("=Welcome BenOS!\r\n");
	uart_send_string("i am benshushu!\r\n");

	printk("running on EL:");
	el = read_sysreg(CurrentEL) >> 2;
	printk("%d\n", el);

	/* init mm */
	mem_init((unsigned long)_ebss, TOTAL_MEMORY);

	/* print mem layout*/
	print_mem();
	printk("\r\n");

	/* check the 0 thread's task_struct address */
	printk("0 thread's task_struct address: 0x%lx\n",
			&init_task_union.task);
	printk("the SP of 0 thread: 0x%lx\n", current_stack_pointer);

	/* test printk */
	printk("el=%3d\n", el);
	printk("el=%-3d\n", el);
	printk("el=%03d\n", el);
	printk("el=0x%-3x\n", el);
	printk("el=0x%03x\n", el);

	printk("0x%hx\n", 0x123456789abcdef);
	printk("0x%x\n", 0x123456789abcdef);
	printk("0x%lx\n", 0x123456789abcdef);
	printk("0x%llx\n", 0x123456789abcdef);

	printk("%d data\n", 0xfffffffe);
	printk("%u data\n", 0xfffffffe);

	printk("%u, %d, %x\n", 1024, -2, -2);

	timer_init();
	raw_local_irq_enable();

	int pid;

	pid = do_fork(PF_KTHREAD, (unsigned long)&kernel_thread, 0);
	if (pid < 0)
		printk("create thread fail\n");

	struct task_struct *next = g_task[pid];

	switch_to(next);

	while (1)
		uart_send(uart_recv());
}
