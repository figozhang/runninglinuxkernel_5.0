#!/usr/bin/python
# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")

from bcc import BPF
from time import sleep

#在finish_task_switch()函数中加入一个kprobe钩子。当系统调用
#finish_task_ switch()函数时会执行我们编写的count_sched()函数
#count_sched()函数实现在task_switch.c文件中。
b = BPF(src_file="task_switch.c")
b.attach_kprobe(event="finish_task_switch", fn_name="count_sched")

# 生成许多调度事件
for i in range(0, 100): sleep(0.01)

for k, v in b["stats"].items():
    print("task_switch[%5d->%5d]=%u" % (k.prev_pid, k.curr_pid, v.value))
