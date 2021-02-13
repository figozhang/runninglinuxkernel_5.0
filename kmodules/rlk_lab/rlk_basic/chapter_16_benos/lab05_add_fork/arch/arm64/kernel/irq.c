#include <mach/irq.h>
#include <asm/io.h>
#include <mach/arm_local_reg.h>
#include <printk.h>

void irq_handle(void)
{
	unsigned int irq = readl(ARM_LOCAL_IRQ_SOURCE0);

	switch (irq) {
	case (CNT_PNS_IRQ):
		handle_timer_irq();
		break;
	default:
		printk("Unknown pending irq: %x\r\n", irq);
	}
}
