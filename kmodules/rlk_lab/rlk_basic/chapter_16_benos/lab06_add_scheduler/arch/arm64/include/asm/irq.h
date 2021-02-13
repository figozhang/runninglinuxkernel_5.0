
static inline void arch_local_irq_enable(void)
{
	asm volatile(
		"msr	daifclr, #2"
		:
		:
		: "memory");
}

static inline void arch_local_irq_disable(void)
{
	asm volatile(
		"msr	daifset, #2"
		:
		:
		: "memory");
}

void arch_irq_handle(void);
