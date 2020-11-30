### 奔跑吧面试考卷

奔跑吧第二版，笨叔精心准备了一份完全不一样的奔跑卷，一份有深度，有热度，有挑战的面试题目，这份卷子和奔跑吧第一版的卷子完全不一样。这里面的题目，都有可能是您以后面试会遇到的题目哟！

1. 如果您答对了90%以上，奔跑吧这本完全不适合您，这本书对您来说太简单了，赶紧把它扔掉或者给身边有需要的小伙伴，你要嘟囔一句：笨叔这个大忽悠，一本烂书！
2. 如果您的跑分在90%以下，说明您对linux内核的认识和理解还有很大提升的空间，奔跑吧这本书完全适合您！奔跑吧第二版为您量身定制哟！
3. 如果您的跑分在30%以下，说明大学里的操作系统课程和计算机体系结构等课程，有可能和笨叔一样逃课、上课睡觉、玩游戏、和女同学聊天了。。。。或者我不是学计算机的，我是转专业的，不过不要紧，奔跑吧入门篇，奔跑吧第二版都适合您！

### 面试考卷

下面一共有20道题，每道题10分，一共200分。读者可以边阅读Linux内核源代码边做题，请在两小时内完成。如没有特殊说明，以下题目基于Linux 5.0内核和ARM64/x86_64架构。

1．请简述数值0x1234 5678在大小端字节序处理器的存储器中的存储方式。

2．假设系统中有4个CPU，每个CPU都有各自的L1高速缓存，处理器内部实现的是MESI协议，它们都想访问相同地址的数据A（大小等于L1高速缓存行大小），这4个CPU的高速缓存在初始状态下都没有缓存数据A。在T0时刻，CPU0访问数据A。在T1时刻，CPU1访问数据A。在T2时刻，CPU2访问数据A。在T3时刻，CPU3想更新数据A的内容。请依次说明，T0～T3时刻，4个CPU各自高速缓存行的变化情况。

3．什么是高速缓存重名问题和同名问题？虚拟索引物理标签（Virtual Index Physical Tag，VIPT）类型的高速缓存在什么情况下会出现高速缓存重名问题？

4．请回答关于页表的几个问题。  
4.1 在ARMv8架构中，在L0～L2页表项中包含了指向下一级页表的基地址，那么这里的下一级页表基地址是物理地址还是虚拟地址？  
4.2 请画出在二级页表架构中从虚拟地址到物理地址查询页表的过程。  
4.3 为什么页表要设计成多级页表？直接使用一级页表是否可行？多级页表又引入了什么问题？  
4.4 内存管理单元（Memory Management Unit，MMU）可以遍历页表，Linux内核也提供了软件遍历页表的函数，如walk_pgd()、follow_page()等。从软件的视角，Linux内核的pgd_t、pud_t、pmd_t以及pte_t数据结构中并没有存储指向下一级页表的指针，它们是如何遍历页表的呢？

5．为用户进程分配物理内存时，分配掩码应该选用GFP_KERNEL，还是GFP_HIGHUSER_ MOVABLE呢？为什么？

6．假设使用printf()输出时指针bufA和bufB指向的地址是一样的，那么在内核中这两块虚拟内存是否冲突了呢？

7．请回答关于缺页异常的几个问题。  
7.1 若发生匿名页面的缺页异常，判断条件是什么？  
7.2 什么是写时复制类型的缺页异常？判断条件是什么？  
7.3 在切换新的PTE（页表项）之前为什么要先把PTE内容清零和刷新TLB？  
7.4 在一个多核的SMP系统中，是否多个CPU内核可以同时对同一个页面发生缺页异常？

8．page数据结构中的_refcount 和_mapcount有什么区别？请列举page数据结构中关于_refcount和_mapcount的案例。

9．在页面分配器中，分配掩码ALLOC_HIGH、ALLOC_HARDER、ALLOC_OOM以及ALLOC_NO_WATERMARKS之间有什么区别？它们各自能访问系统预留内存的百分比是多少？思考为什么页面分配器需要做这样的安排。

10．假设有这样的场景——请求分配order为4的一块内存，迁移类型是不可迁移
（MIGRATE_UNMOVABLE），但是order大于或等于4的不可迁移类型的空闲链表中没有空闲页块，那么页面分配器会怎么办？

11．把/proc/meminfo节点中SwapTotal减去SwapFree等于系统中已经使用的交换内存大小，我们称之为S_swap。写一个小程序来遍历系统中所有的进程，并把进程中/proc/PID/status节点的VmSwap值都累加起来，我们把它称为P_swap，为什么这两个值不相等？

12．请简述fork()、vfork()和clone()之间的区别。在ARM64的Linux内核中如何获取当前进程的task_struct数据结构？

13．请回答关于负载计算的几个问题。  
13.1 如何理解负载、量化负载、权重、优先级、额定算力、实际算力、衰减等概念？  
13.2 在PELT算法中，如何计算第n个周期的衰减？  
13.3 在PELT算法中，如何计算一个进程的可运行状态的量化负载（load_avg）？  
13.4 在PELT算法中，如何计算一个调度队列的可运行状态的量化负载（runnable_load_avg）？  
13.5 在PELT算法中，如何计算一个进程的实际算力（util_avg）？

14．假设进程A和进程B都是在用户空间运行的两个进程，它们不主动陷入内核态。  
14.1 调度器准备调度进程B来运行，在做进程切换时需要做什么事情才能把进程A切换到进程B？  
14.2 进程B开始运行时，它从什么地方开始运行第一条指令，是直接运行暂停在用户空间的那条指令吗？为什么？

15．案例分析：在一个双核处理器的系统中，在Shell界面下运行test程序。CPU0的就绪队列中有4个进程，而CPU1的就绪队列中有1个进程。test程序和这5个进程的nice值都为0。  
15.1 请画出test程序在内核空间的运行流程图。  
15.2 运行test程序一段时间之后，CPU0和CPU1的就绪队列有什么变化？

16．假设CPU0先持有了自旋锁，接着CPU1、CPU2、CPU3都加入该锁的争用中，请阐述这几个CPU如何获取锁，并画出它们申请锁的流程图。

17．假设CPU0～CPU3同时争用一个互斥锁，CPU0率先申请了互斥锁，然后CPU1也加入锁的申请。CPU1在持有锁期间会进入睡眠状态。然后CPU2和CPU3陆续加入该锁的争用中。请画出这几个CPU争用锁的时序图。

18．为什么中断上下文不能运行睡眠操作？软中断的回调函数运行过程中是否允许响应本地中断？是否允许同一个tasklet在多个CPU上并行运行？

19．请回答关于链接的几个问题。  
19.1 什么是加载地址、运行地址和链接地址？  
19.2 在实际项目开发中，为什么要刻意设置加载地址、运行地址以及链接地址不一样呢？  
19.3 什么是重定位？  
19.4 什么是位置无关的汇编指令？什么是位置有关的汇编指令？

20．假设函数调用关系为main()->func1()->func2()，请画出x84_64架构或者ARM64架构的函数栈的布局图。

### 答案
以上题目的答案都分布在《奔跑吧Linux内核 * 第二版》的各章中。
