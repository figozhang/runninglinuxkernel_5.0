#include <uapi/linux/ptrace.h>
#include <linux/sched.h>

struct key_t {
    u32 prev_pid;
    u32 curr_pid;
};

//BPF_HASH()函数用来创建一个BPF Map对象，这个对象由哈希表组成，类似于一个数组,
//它由1024个key_t类型的成员组成。
BPF_HASH(stats, struct key_t, u64, 1024);

/*
 * 当finish_task_switch()函数被调用时会执行count_sched()函数
 */
int count_sched(struct pt_regs *ctx, struct task_struct *prev) {
    struct key_t key = {};
    u64 zero = 0, *val;

    /*
     * 把当前进程的ID存储到key.curr_pid，key.prev_pid存储了前一个进程的ID。
     */
    key.curr_pid = bpf_get_current_pid_tgid();
    key.prev_pid = prev->pid;

    /*
     * 根据key来查找哈希表。若有相同的key，则增加val；若没有，则把key添加到哈希表中。
     */
    val = stats.lookup_or_try_init(&key, &zero);
    if (val) {
        (*val)++;
    }
    return 0;
}
