#include <asm/mm.h>
#include <asm/sysregs.h>
#include <sched.h>

.section  .rodata
.align 3
.globl string1
string1:
	.string "Booting at EL"

sp_addr:
	.quad init_task_union +THREAD_START_SP

.section ".text.boot"
.globl _start
_start:
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF		// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU
	b	proc_hang

proc_hang:
	b 	proc_hang

master:
#ifdef CONFIG_DEBUG_ON_EARLY_ASM
        /* init uart and print the string*/
	bl __init_uart
#endif

	mrs x5, CurrentEL
	cmp x5, #CurrentEL_EL3
	b.eq el3_entry
	b el2_entry

el3_entry:
#ifdef CONFIG_DEBUG_ON_EARLY_ASM
	bl print_el
#endif
	ldr x0, =SCTLR_EL2_VALUE_MMU_DISABLED
	msr sctlr_el2, x0

	ldr x0, =HCR_HOST_NVHE_FLAGS
	msr hcr_el2, x0

	ldr x0, =SCR_VALUE
	msr scr_el3, x0

	ldr x0, =SPSR_EL2
	msr spsr_el3, x0

	adr x0, el2_entry
	msr elr_el3, x0

	eret

el2_entry:
#ifdef CONFIG_DEBUG_ON_EARLY_ASM
	bl print_el
#endif
	ldr x0, =SCTLR_EL2_VALUE_MMU_DISABLED
	msr sctlr_el2, x0

	/* The Execution state for EL1 is AArch64 */
	ldr x0, =HCR_HOST_NVHE_FLAGS
	msr hcr_el2, x0

	ldr x0, =SCTLR_EL1_VALUE_MMU_DISABLED
	msr sctlr_el1, x0

	ldr x0, =SPSR_EL1
	msr spsr_el2, x0

	adr x0, el1_entry
	msr elr_el2, x0

	eret

el1_entry:
#ifdef CONFIG_DEBUG_ON_EARLY_ASM
	bl print_el
#endif
	/* 设置异常向量表基地址到vbar寄存器 */
	ldr     x5, =vectors
	msr     vbar_el1, x5
	isb

	adr x0, bss_begin
	adr x1, bss_end
	sub x1, x1, x0
	bl memzero

	/* set sp to top of init_task_union */
	ldr x8, =sp_addr
	ldr x9, [x8]
	mov sp, x9

	bl kernel_main
	b proc_hang // should never come here

print_el:
	mov x10, x30

	/* print EL */
	ldr x0, =string1
	bl put_string_uart

	mrs x5, CurrentEL
	/* get the currentEL value */
	lsr x2, x5, #2
	mov x0, #48
	add x0, x0, x2
	bl put_uart
	/* print the new line tab */
	mov x0, #10
	bl put_uart

	mov x30, x10
	ret
