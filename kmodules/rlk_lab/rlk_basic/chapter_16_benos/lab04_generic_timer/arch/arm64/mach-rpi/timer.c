#include <mach/timer.h>
#include <mach/irq.h>
#include <asm/io.h>
#include <printk.h>
#include <mach/arm_local_reg.h>
#include <timer.h>

static unsigned int val = NSEC_PER_SEC / HZ;

static int generic_timer_init(void)
{
	asm volatile(
		"mov x0, #1\n"
		"msr cntp_ctl_el0, x0"
		:
		:
		: "memory");

	return 0;
}

static int generic_timer_reset(unsigned int val)
{
	asm volatile(
		"msr cntp_tval_el0, %x[timer_val]"
		:
		: [timer_val] "r" (val)
		: "memory");

	return 0;
}

static void enable_timer_interrupt(void)
{
	writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

void timer_init(void)
{
	generic_timer_init();
	generic_timer_reset(val);

	enable_timer_interrupt();
}

void handle_timer_irq(void)
{
	generic_timer_reset(val);
	printk("Core0 Timer interrupt received\r\n");
}
