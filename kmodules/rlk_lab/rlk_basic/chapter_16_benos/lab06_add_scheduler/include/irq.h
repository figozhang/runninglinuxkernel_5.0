#include <asm/irq.h>

#define raw_local_irq_disable()	arch_local_irq_disable()
#define raw_local_irq_enable()	arch_local_irq_enable()

