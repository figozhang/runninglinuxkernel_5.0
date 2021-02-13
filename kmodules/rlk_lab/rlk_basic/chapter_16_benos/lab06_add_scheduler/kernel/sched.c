#include <irq.h>
#include <sched.h>
#include <printk.h>

/* 定义一个全局的就绪队列*/
struct run_queue g_rq;

struct task_struct *pick_next_task(struct run_queue *rq,
		struct task_struct *prev)
{
	const struct sched_class *class = &simple_sched_class;

	return class->pick_next_task(rq, prev);
}

void dequeue_task(struct run_queue *rq, struct task_struct *p)
{
	const struct sched_class *class = &simple_sched_class;

	return class->dequeue_task(rq, p);
}

void enqueue_task(struct run_queue *rq, struct task_struct *p)
{
	const struct sched_class *class = &simple_sched_class;

	return class->enqueue_task(rq, p);
}

void task_tick(struct run_queue *rq, struct task_struct *p)
{
	const struct sched_class *class = &simple_sched_class;

	return class->task_tick(rq, p);
}

struct task_struct * switch_to(struct task_struct *prev,
		struct task_struct *next)
{
	if (prev == next)
		return NULL;

	return cpu_switch_to(prev, next);
}

/*
 * 处理调度完成后的一些收尾工作，由next进程来收拾
 * prev进程的烂摊子
 *
 * Note: 新创建进程第一次运行也会调用该函数来处理
 * prev进程的烂摊子
 * ret_from_fork->schedule_tail
 */
void schedule_tail(struct task_struct *prev)
{
	/* 打开中断 */
	raw_local_irq_enable();
}

void tick_handle_periodic(void)
{
	struct run_queue *rq = &g_rq;

	task_tick(rq, current);
}

/* 检查是否在中断上下文里发生了调度，这是一个不好
 * 的习惯。因为中断上下文通常是关中断的，若发生
 * 调度了，CPU选择了next进程运行，CPU就运行在
 * next进程中，那么可能长时间处于关中断状态，这样
 * 时钟tick有可能丢失，导致系统卡住了
 */
static void schedule_debug(struct task_struct *p)
{
	if (in_atomic_preempt_off())
		printk("BUG: scheduling while atomic: %d, 0x%x\n",
				p->pid, preempt_count());
}

static void __schedule(void)
{
	struct task_struct *prev, *next, *last;
	struct run_queue *rq = &g_rq;

	prev = current;

	/* 检查是否在中断上下文中发生了调度 */
	schedule_debug(prev);

	/* 关闭中断包含调度器，以免中断发生影响调度器 */
	raw_local_irq_disable();

	if (prev->state)
		dequeue_task(rq, prev);

	next = pick_next_task(rq, prev);
	clear_task_resched(prev);
	if (next != prev) {
		last = switch_to(prev, next);
		/*
		 * switch_to函数是用来切换prev进程到next进程。
		 * switch_to函数执行完成之后，已经切换到next
		 * 进程,整个栈和时空都发生变化了,不能使用这
		 * 里的prev变量来表示prev进程，只能通过aarch64
		 * 的x0寄存器来获取prev进程的task_struct。
		 *
		 * 在switch_to函数使用x0寄存器来传递prev进程
		 * task_struct,返回值也是通过x0寄存器，因此
		 * 这里last变量表示prev进程的task_struct
		 */
		rq->nr_switches++;
		rq->curr = current;
	}

	/* 由next进程来收拾prev进程的现场 */
	schedule_tail(last);
}

/* 普通调度 */
void schedule(void)
{
	/* 关闭抢占，以免嵌套发生调度抢占*/
	preempt_disable();
	__schedule();
	preempt_enable();
}

/* 抢占调度
 *
 * 中断返回前会检查是否需要抢占调度
 */
void preempt_schedule_irq(void)
{
	/* this must be preemptible now*/
	if (preempt_count())
		printk("BUG: %s incorrect preempt count: 0x%x\n",
				__func__, preempt_count());

	/* 关闭抢占，以免嵌套发生调度抢占*/
	preempt_disable();
	/*
	 * 这里打开中断，处理高优先级中断，
	 * 中断比抢占调度优先级。
	 *
	 * 若这里发生中断，中断返回后，前面关闭抢占
	 * 不会嵌套发生抢占调度
	 */
	raw_local_irq_enable();
	__schedule();
	raw_local_irq_disable();
	preempt_enable();
}

void wake_up_process(struct task_struct *p)
{
	struct run_queue *rq = &g_rq;

	p->state = TASK_RUNNING;

	enqueue_task(rq, p);
}

void sched_init(void)
{
	struct run_queue *rq = &g_rq;

	INIT_LIST_HEAD(&rq->rq_head);
	rq->nr_running = 0;
	rq->nr_switches = 0;
	rq->curr = NULL;
}
