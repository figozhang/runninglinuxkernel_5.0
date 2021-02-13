/*
 * Based on arch/arm/kernel/asm-offsets.c
 *
 * Copyright (C) 1995-2003 Russell King
 *               2001-2002 Keith Owens
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <target/kbuild.h>
#include <target/compiler.h>
#include <sched.h>

int main(void)
{
	DEFINE(S_FRAME_SIZE, sizeof(struct pt_regs));
	DEFINE(S_X0,	offsetof(struct pt_regs, regs[0]));
	DEFINE(S_X1,	offsetof(struct pt_regs, regs[1]));
	DEFINE(S_X2,	offsetof(struct pt_regs, regs[2]));
	DEFINE(S_X3,	offsetof(struct pt_regs, regs[3]));
	DEFINE(S_X4,	offsetof(struct pt_regs, regs[4]));
	DEFINE(S_X5,	offsetof(struct pt_regs, regs[5]));
	DEFINE(S_X6,	offsetof(struct pt_regs, regs[6]));
	DEFINE(S_X7,	offsetof(struct pt_regs, regs[7]));
	DEFINE(S_X8,	offsetof(struct pt_regs, regs[8]));
	DEFINE(S_X10,	offsetof(struct pt_regs, regs[10]));
	DEFINE(S_X12,	offsetof(struct pt_regs, regs[12]));
	DEFINE(S_X14,	offsetof(struct pt_regs, regs[14]));
	DEFINE(S_X16,	offsetof(struct pt_regs, regs[16]));
	DEFINE(S_X18,	offsetof(struct pt_regs, regs[18]));
	DEFINE(S_X20,	offsetof(struct pt_regs, regs[20]));
	DEFINE(S_X22,	offsetof(struct pt_regs, regs[22]));
	DEFINE(S_X24,	offsetof(struct pt_regs, regs[24]));
	DEFINE(S_X26,	offsetof(struct pt_regs, regs[26]));
	DEFINE(S_X28,	offsetof(struct pt_regs, regs[28]));
	DEFINE(S_FP,	offsetof(struct pt_regs, regs[29]));
	DEFINE(S_LR,	offsetof(struct pt_regs, regs[30]));
	DEFINE(S_SP,	offsetof(struct pt_regs, sp));
	DEFINE(S_PSTATE, offsetof(struct pt_regs, pstate));
	DEFINE(S_PC,	 offsetof(struct pt_regs, pc));
	DEFINE(S_ORIG_X0, offsetof(struct pt_regs, orig_x0));
	DEFINE(S_SYSCALLNO, offsetof(struct pt_regs, syscallno));
	DEFINE(S_ORIG_ADDR_LIMIT, offsetof(struct pt_regs, orig_addr_limit));
	DEFINE(S_STACKFRAME, offsetof(struct pt_regs, stackframe));

	DEFINE(THREAD_CPU_CONTEXT, offsetof(struct task_struct, cpu_context));
	DEFINE(TI_PREEMPT,  offsetof(struct task_struct, preempt_count));
	DEFINE(NEED_RESCHED, offsetof(struct task_struct, need_resched));
	return 0;
}
