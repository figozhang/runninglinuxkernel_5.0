
#ifndef _P_IO_H
#define _P_BASE_H

#if 1
#define __arch_getl(a)			(*(volatile unsigned int *)(a))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))

#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()

#define readl(c)	({ unsigned int  __v = __arch_getl(c); __iormb(); __v; })
#define writel(v,c)	({ unsigned int  __v = v; __iowmb(); __arch_putl(__v,c);})
#else
static inline void writel(unsigned int value, unsigned int addr)
{
	*(volatile unsigned int *)addr = value;
}

static inline unsigned int readl(unsigned int addr)
{
	return *(volatile unsigned int *)addr;
}
#endif

static inline void delay(unsigned int n)
{
	while(n--)
		;
}

#endif
