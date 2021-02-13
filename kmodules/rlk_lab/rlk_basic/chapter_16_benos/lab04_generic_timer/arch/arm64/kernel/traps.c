#include <asm/system.h>
#include <printk.h>

static const char * const bad_mode_handler[] = {
	"Sync Abort",
	"IRQ",
	"FIQ",
	"Error"
};

void bad_mode(struct pt_regs *regs, int reason, unsigned int esr)
{
	printk("Bad mode for %s handler detected, esr=0x%x\n",
			bad_mode_handler[reason], esr);

	while (1)
		;
}
