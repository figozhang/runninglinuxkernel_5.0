#!/bin/sh

#创建一个日志目录
mkdir -p /data/rlk/

#打开内核所有log等级
#echo 8 > /proc/sys/kernel/printk

#确保该脚本不会被OOM终止
echo -1000 > /proc/self/oom_score_adj
cd /sys/kernel/debug/tracing

#先暂停ftrace
echo 0 > tracing_on
#清空跟踪缓冲区
echo  > trace

#打开与OOM和vmscan相关的跟踪事件
echo 1 > /sys/kernel/debug/tracing/events/oom/oom_score_adj_update/enable
echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_shrink_slab_start/enable
echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_shrink_slab_end/enable

#开始采集数据
echo 1 > tracing_on

TIMES=0
while true
do     
        dmesg | grep "invoked oom-killer"   #这是判断问题的触发条件
        if [ $? -eq 0 ]
        then
                 #保存ftrace 日志和内核日志
                 cat /sys/kernel/debug/tracing/trace > /data/rlk/ftrace_log0.txt.$TIMES
                 dmesg > /data/rlk/kmsg.txt.$TIMES
                 let TIMES+=1

                 #清空kernel log和ftrace log，等待下一次条件触发
                 dmesg -c
                 echo > trace
        fi
        sleep 1
done

