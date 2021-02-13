#include <irq.h>
#include <sched.h>

void irq_handle(void)
{
	__irq_enter();
	arch_irq_handle();
	__irq_exit();
}
