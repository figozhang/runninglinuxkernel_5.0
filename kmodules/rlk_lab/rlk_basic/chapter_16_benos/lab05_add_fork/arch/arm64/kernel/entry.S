#include <asm/asm-offsets.h>

#define BAD_SYNC        0
#define BAD_IRQ         1
#define BAD_FIQ         2
#define BAD_ERROR       3

/*
   vector table entry
   每个表项是128字节， align 7表示128字节对齐
 */
	.macro vtentry label
	.align 7
	b \label
	.endm

/*
   处理无效的异常向量
 */
	.macro inv_entry el, reason
	kernel_entry el
	mov x0, sp
	mov x1, #\reason
	mrs x2, esr_el1
	b bad_mode
	.endm

/*
   保存异常发生时候的上下文

   保存x0~x29，x30（lr），sp, elr, spsr保存到 栈中
 */
	.macro kernel_entry el
	/*
	   SP指向了栈底, S_FRAME_SIZE表示一个栈框的大小.
	   定义一个struct pt_regs来描述一个栈框,
	   用在异常发生时保存上下文.
	 */
	sub sp, sp, #S_FRAME_SIZE

	/*
	   保存通用寄存器x0~x29到栈框里pt_regs->x0~x29
	 */
	stp x0, x1, [sp, #16 *0]
	stp x2, x3, [sp, #16 *1]
	stp x4, x5, [sp, #16 *2]
	stp x6, x7, [sp, #16 *3]
	stp x8, x9, [sp, #16 *4]
	stp x10, x11, [sp, #16 *5]
	stp x12, x13, [sp, #16 *6]
	stp x14, x15, [sp, #16 *7]
	stp x16, x17, [sp, #16 *8]
	stp x18, x19, [sp, #16 *9]
	stp x20, x21, [sp, #16 *10]
	stp x22, x23, [sp, #16 *11]
	stp x24, x25, [sp, #16 *12]
	stp x26, x27, [sp, #16 *13]
	stp x28, x29, [sp, #16 *14]

	/* x21: 栈顶 的位置*/
	add     x21, sp, #S_FRAME_SIZE

	mrs     x22, elr_el1
	mrs     x23, spsr_el1

	/* 把lr保存到pt_regs->lr, 把sp保存到pt_regs->sp位置*/
	stp     lr, x21, [sp, #S_LR]
	/* 把elr_el1保存到pt_regs->pc中
	   把spsr_elr保存到pt_regs->pstate中*/
	stp     x22, x23, [sp, #S_PC]
	.endm

/*
   恢复异常发生时保存下来的上下文
 */
	.macro kernel_exit el
	/* 从pt_regs->pc中恢复elr_el1,
	   从pt_regs->pstate中恢复spsr_el1
	 */
	ldp     x21, x22, [sp, #S_PC]           // load ELR, SPSR
	msr     elr_el1, x21                    // set up the return data
	msr     spsr_el1, x22

	ldp     x0, x1, [sp, #16 * 0]
	ldp     x2, x3, [sp, #16 * 1]
	ldp     x4, x5, [sp, #16 * 2]
	ldp     x6, x7, [sp, #16 * 3]
	ldp     x8, x9, [sp, #16 * 4]
	ldp     x10, x11, [sp, #16 * 5]
	ldp     x12, x13, [sp, #16 * 6]
	ldp     x14, x15, [sp, #16 * 7]
	ldp     x16, x17, [sp, #16 * 8]
	ldp     x18, x19, [sp, #16 * 9]
	ldp     x20, x21, [sp, #16 * 10]
	ldp     x22, x23, [sp, #16 * 11]
	ldp     x24, x25, [sp, #16 * 12]
	ldp     x26, x27, [sp, #16 * 13]
	ldp     x28, x29, [sp, #16 * 14]

	/* 从pt_regs->lr中恢复lr*/
	ldr     lr, [sp, #S_LR]
	add     sp, sp, #S_FRAME_SIZE           // restore sp
	eret                                    // return to kernel
	.endm


/*
 * Vector Table
 *
 * ARM64的异常向量表一共占用2048个字节
 * 分成4组，每组4个表项，每个表项占128字节
 * 参见ARMv8 spec v8.6第D1.10节
 * align 11表示2048字节对齐
 */
.align 11
.global vectors
vectors:
	/* Current EL with SP0
	   当前系统运行在EL1时使用EL0的栈指针SP
	   这是一种异常错误的类型
	 */
	vtentry el1_sync_invalid
	vtentry el1_irq_invalid
	vtentry el1_fiq_invalid
	vtentry el1_error_invalid

	/* Current EL with SPx
	   当前系统运行在EL1时使用EL1的栈指针SP
	   这说明系统在内核态发生了异常

	   Note: 我们暂时只实现IRQ中断
	 */
	vtentry el1_sync_invalid
	vtentry el1_irq
	vtentry el1_fiq_invalid
	vtentry el1_error_invalid

	/* Lower EL using AArch64
	   在用户态的aarch64的程序发生了异常
	 */
	vtentry el0_sync_invalid
	vtentry el0_irq_invalid
	vtentry el0_fiq_invalid
	vtentry el0_error_invalid

	/* Lower EL using AArch32
	   在用户态的aarch32的程序发生了异常
	 */
	vtentry el0_sync_invalid
	vtentry el0_irq_invalid
	vtentry el0_fiq_invalid
	vtentry el0_error_invalid

el1_sync_invalid:
	inv_entry 1, BAD_SYNC
el1_irq_invalid:
	inv_entry 1, BAD_IRQ
el1_fiq_invalid:
	inv_entry 1, BAD_FIQ
el1_error_invalid:
	inv_entry 1, BAD_ERROR
el0_sync_invalid:
	inv_entry 0, BAD_SYNC
el0_irq_invalid:
	inv_entry 0, BAD_IRQ
el0_fiq_invalid:
	inv_entry 0, BAD_FIQ
el0_error_invalid:
	inv_entry 0, BAD_ERROR

el1_irq:
	kernel_entry 1
	bl irq_handle
	kernel_exit 1

/*
   对于用户进程，暂时还没实现
 */
.align 2
.global ret_to_user
ret_to_user:
	inv_entry 0, BAD_ERROR

/*
 进程fork之后第一次进程切换
 对于内核线程：
    x19保存了进程回调函数的入口
    x20保存进程的回调函数的参数
 */
.align 2
.global ret_from_fork
ret_from_fork:
	cbz x19, 1f
	mov x0, x20
	blr x19
1:
	b ret_to_user

/*
进程切换： 保存prev进程的上下文，并且恢复next进程
的上下文
  cpu_switch_to(struct task_struct *prev,
	   struct task_struct *next);

需要保存的上下文： x19 ~ x29， sp， lr
保存到进程的task_struct->cpu_context
 */
.align
.global cpu_switch_to
cpu_switch_to:
	add     x8, x0, #THREAD_CPU_CONTEXT
	mov     x9, sp
	stp     x19, x20, [x8], #16
	stp     x21, x22, [x8], #16
	stp     x23, x24, [x8], #16
	stp     x25, x26, [x8], #16
	stp     x27, x28, [x8], #16
	stp     x29, x9, [x8], #16
	str     lr, [x8]

	add     x8, x1, #THREAD_CPU_CONTEXT
	ldp     x19, x20, [x8], #16
	ldp     x21, x22, [x8], #16
	ldp     x23, x24, [x8], #16
	ldp     x25, x26, [x8], #16
	ldp     x27, x28, [x8], #16
	ldp     x29, x9, [x8], #16
	ldr     lr, [x8]
	mov     sp, x9
	ret
