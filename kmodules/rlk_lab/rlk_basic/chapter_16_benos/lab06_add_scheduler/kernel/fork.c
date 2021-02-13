#include <sched.h>
#include <string.h>
#include <memory.h>

/* 把0号进程的内核栈 编译链接到.data.init_task段中 */
#define __init_task_data __attribute__((__section__(".data.init_task")))

/* 0号进程为init进程 */
union task_union init_task_union __init_task_data
			= {INIT_TASK(init_task_union.task)};

/* 定义一个全局的task_struct数组来存放进程的PCB*/
struct task_struct *g_task[NR_TASK] = {&init_task_union.task,};

unsigned long total_forks;

static int find_empty_task(void)
{
	int i;

	for (i = 0 ; i < NR_TASK; i++) {
		if (!g_task[i])
			return i;
	}

	return -1;
}

/*
 * pt_regs存储在栈顶
 */
static struct pt_regs *task_pt_regs(struct task_struct *tsk)
{
	unsigned long p;

	p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);

	return (struct pt_regs *)p;
}

/*
 * 设置子进程的上下文信息
 */
static int copy_thread(unsigned long clone_flags, struct task_struct *p,
		unsigned long fn, unsigned long arg)
{
	struct pt_regs *childregs;

	childregs = task_pt_regs(p);
	memset(childregs, 0, sizeof(struct pt_regs));
	memset(&p->cpu_context, 0, sizeof(struct cpu_context));

	if (clone_flags & PF_KTHREAD) {
		childregs->pstate = PSR_MODE_EL1h;
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	}

	p->cpu_context.pc = (unsigned long)ret_from_fork;
	p->cpu_context.sp = (unsigned long)childregs;

	return 0;
}

/*
 * fork一个新进程
 * 1. 新建一个task_strut。 分配4KB页面用来存储内核栈,
 * task_struct在栈底。
 * 2. 分配PID
 * 3. 设置进程的上下文
 */
int do_fork(unsigned long clone_flags, unsigned long fn, unsigned long arg)
{
	struct task_struct *p;
	int pid;

	p = (struct task_struct *)get_free_page();
	if (!p)
		goto error;

	memset(p, 0, sizeof(*p));

	pid = find_empty_task();
	if (pid < 0)
		goto error;

	if (copy_thread(clone_flags, p, fn, arg))
		goto error;

	p->state = TASK_RUNNING;
	p->pid = pid;
	p->counter = (current->counter + 1) >> 1;
	current->counter >>= 1;
	p->need_resched = 0;
	p->preempt_count = 0;
	p->priority = 2;
	total_forks++;
	g_task[pid] = p;
	SET_LINKS(p);
	wake_up_process(p);

	return pid;

error:
	return -1;
}
