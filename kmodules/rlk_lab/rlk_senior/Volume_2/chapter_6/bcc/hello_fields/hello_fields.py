#!/usr/bin/python
#
# This is a Hello World example that formats output as fields.

from bcc import BPF
from bcc.utils import printb

# 定义 BPF 程序
prog = """
int hello(void *ctx) {
    bpf_trace_printk("Hello, World!\\n");
    return 0;
}
"""

# 在系统调用函数sys_clone()时加入一个kprobe钩子，当系统执行到sys_clone()时会调用刚才定义的hello函数。
b = BPF(text=prog)
b.attach_kprobe(event=b.get_syscall_fnname("clone"), fn_name="hello")

# header
print("%-18s %-16s %-6s %s" % ("TIME(s)", "COMM", "PID", "MESSAGE"))

# format output
while 1:
    try:
        (task, pid, cpu, flags, ts, msg) = b.trace_fields()
    except KeyboardInterrupt:
        exit()
    printb(b"%-18.9f %-16s %-6d %s" % (ts, task, pid, msg))
