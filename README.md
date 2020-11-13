# 《奔跑吧Linux内核》 第二版配套实验平台

## 《奔跑吧Linux内核》的来由
笨叔当fae的时候，去请教研发兄弟一些问题，时常会遭到白眼：我也不太懂，某某代码里有，你自己看啦，某某芯片手册有，你自己看啦。
现在人生活压力忒大，生怕教会了别人，感觉明天就会失业了一样。笨叔觉得，好东西需要记录下来和大家分享，于是有了《奔跑吧linux内核》。

## 实验平台说明
本git repo是《奔跑吧Linux内核》一书的配套实验平台，包含Linux 5.0代码以及书上实验参考代码。
使用busybox工具制作的最小文件系统，该最小系统仅仅包含了Linux系统最常用的命令，如ls，top等命令。如果要在此最小系统中进行systemtap以及kdump等试验的话，
我们需要手动编译和安装这些工具，这个过程是相当复杂和繁琐的。为此，我们尝试使用Debian的根文件系统来构造一个小巧而且好用的实验平台。
在这个实验平台中，读者可以在线安装丰富的软件包，比如kdump，crash，systemtap等工具。这个实验平台具有如下特点：
1. 使用“O0”来编译内核
2. 在主机Host中编译内核
3. 使用QEMU来加载系统
4. 支持GDB单步调试内核以及Debian系统
5. 使用ARM64版本的Debian系统的根文件系统
6. 在线安装Debian软件包
7. 支持在虚拟机里动态编译内核模块
8. 支持Host主机和虚拟机共享文件

本书推荐的实验环境如下。
1. 主机硬件平台：Intel x86_84处理器兼容主机。
2. 主机操作系统：Ubuntu Linux 20.04

在Ubuntu Linux 20.04可以通过如下命令来安装本书需要的软件包。

```
$ sudo apt update -y
$ sudo apt install net-tools libncurses5-dev libssl-dev build-essential openssl qemu-system-arm libncurses5-dev gcc-aarch64-linux-gnu git bison flex bc vim universal-ctags cscope cmake python3-dev gdb-multiarch openjdk-13-jre trace-cmd kernelshark bpfcc-tools cppcheck docker docker.io
```

## 本书配套的其他资源
请关注奔跑吧linux社区微信公众号：runninglinuxkernel

## 本书配套的视频课程
若读者觉得阅读本书的文字版本不过瘾，想看笨叔录制的视频课程，请关注：
1. 淘宝店：https://shop115683645.taobao.com/
2. 微店：https://weidian.com/?userid=1692165428

目前上线的配套课程有3个：
1. 第一季旗舰篇：Linux内存管理
![image](https://benshushu.coding.net/p/runninglinuxkernel_5.0/d/runninglinuxkernel_5.0/git/raw/rlk_5.0/images/1rd_training.jpg)
2. 第二季旗舰篇：Linux进程管理、中断管理、锁机制三合一
![image](https://benshushu.coding.net/p/runninglinuxkernel_5.0/d/runninglinuxkernel_5.0/git/raw/rlk_5.0/images/2rd-training.jpg)
3. 第三季旗舰篇：ARM64体系结构与编程

全球原创的ARM64实验，全球首个手把手解读ARMv8芯片手册，笨叔带您一起做实验，一起玩树莓派，一起进步！
![image](https://benshushu.coding.net/p/runninglinuxkernel_5.0/d/runninglinuxkernel_5.0/git/raw/rlk_5.0/images/arm64_training.png)

## 使用O0编译的内核的好处
这个runninglinuxkernel内核默认使用GCC的“O0”优化等级来编译的。
读者可能发现gdb在单步调试内核时会出现光标乱跳并且无法打印有些变量的值（例如出现<optimized out>）等问题，其实这不是gdb或QEMU的问题。
是因为内核编译的默认优化选项是O2，因此如果不希望光标乱跳，可以尝试把linux-5.0根目录Makefile中的O2改成O0，但是这样编译时有问题，作者为此做了一些修改。
最后需要特别说明一下，使用GCC的“O0”优化等级编译内核会导致内核运行性能下降，因此我们仅仅是为了方便单步调试内核。
使用O1或者O2编译的内核，使用gdb进行单步调试时查看变量会出现大量的<optimized out>。

## 实验说明
本书配套的实验代码是在kmodules/rlk_basic目录下面。


### 1. 编译运行
在runninglinuxkernel_5.0目录下面有一个rootfs_arm64.tar.xz文件，这个是采用Ubuntu/Debian系统的根文件系统制作而成。
但是，这个根文件系统还只是一个半成品，我们还需要根据编译好的内核来安装内核镜像和内核模块，整个过程比较复杂。
1. 编译内核。
2. 编译内核模块。
3. 安装内核模块。
4. 安装内核头文件。
5. 安装编译内核模块必须依赖文件。
6. 制作ext4根文件系统。
这个过程比较繁琐，我们制作了一个脚本来简化上述过程。
注意，该脚本会使用dd命令来生成一个4GB大小的镜像文件，因此主机系统需要保证至少10 GB的空余磁盘空间。若读者需要生成一个更大的根文件系统镜像，可以修改run_rlk_arm64.sh这个脚本文件。
首先，编译内核。
```
$ cd runninglinuxkernel_5.0
$ ./run_debian_arm64.sh build_kernel
```

执行上述脚本需要几十分钟，依赖于主机的计算能力。接着，编译根文件系统。
```
$ cd runninglinuxkernel_5.0
$ sudo ./run_debian_arm64.sh build_rootfs
```

读者需要注意，编译根文件系统需要管理员权限，而编译内核则不需要。执行完成后会生成一个名为rootfs_arm64.ext4的根文件系统。

运行刚才编译好的ARM64版本的Linux系统。
运行run_debian_arm64.sh脚本，输入run参数即可。
```
$./run_debian_arm64.sh run
```
系统登录名: benshushu
密码：123

切换到root用户：su root

如果读者修改了runninglinuxkernel_5.0内核的配置文件，比如arch/arm64/config/debian_defconfig文件，那么需要重新编译内核以及更新根文件系统。
```
$ ./run_debian_arm64.sh build_kernel         # 重新编译内核
$ sudo ./run_debian_arm64.sh update_rootfs  #更新根文件系统
```

### 2. 在线安装软件包
QEMU虚拟机可以通过VirtIO-Net技术来生成一个虚拟的网卡，通过NAT网络桥接技术和主机进行网络共享。使用ifconfig命令来检查网络配置。
```
root@ubuntu:~# ifconfig
enp0s1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.0.2.15  netmask 255.255.255.0  broadcast 10.0.2.255
        inet6 fec0::ce16:adb:3e70:3e71  prefixlen 64  scopeid 0x40<site>
        inet6 fe80::c86e:28c4:625b:2767  prefixlen 64  scopeid 0x20<link>
        ether 52:54:00:12:34:56  txqueuelen 1000  (Ethernet)
        RX packets 23217  bytes 33246898 (31.7 MiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4740  bytes 267860 (261.5 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 2  bytes 78 (78.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 2  bytes 78 (78.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```
可以看到生成了一个名为enp0s1的网卡设备，分配的IP地址为：10.0.2.15。
通过apt update命令来更新Debian系统的软件仓库。
```
root@ubuntu:~# apt update
```
如果更新失败，有可能是系统时间比较旧了，可以使用date命令来设置日期。
```
root@ubuntu:~# date -s 2020-03-29 #假设最新日期是2020年3月29日
Sun Mar 29 00:00:00 UTC 2020
```
使用apt install命令来安装软件包。比如，可以在线安装gcc。
```
root@ubuntu:~# apt install gcc
```
### 3. 共享文件夹
在主机和QEMU虚拟机之间共享文件。
主机和QEMU虚拟机可以通过NET_9P技术进行文件共享，这个需要QEMU虚拟机和主机的Linux内核都使能NET_9P的内核模块。本实验平台已经支持主机和QEMU虚拟机的共享文件，可以通过如下简单方法来测试。
复制一个文件到runninglinuxkernel_5.0/kmodules目录下面。
```
$ cp test.c  runninglinuxkernel_5.0/kmodules
```
启动QEMU虚拟机之后，首先检查一下/mnt目录是否有test.c文件。
```
root@ubuntu:/# cd /mnt
root@ubuntu:/mnt # ls
README     test.c
```
我们在后续的实验中会经常利用这个特性，比如把编译好的内核模块或者内核模块源代码放入QEMU虚拟机。
### 4. 编译内核模块
在QEMU虚拟机中安装必要的软件包。
```
root@ubuntu: # apt install build-essential
```

在QEMU虚拟机里编译内核模块时需要指定QEMU虚拟机本地的内核路径，例如BASEINCLUDE变量指向了本地内核路径。“/lib/modules/$(shell uname -r)/build”是一个链接文件，用来指向具体内核源代码路径，通常是指向已经编译过的内核路径。
```
BASEINCLUDE ?= /lib/modules/$(shell uname -r)/build
```

编译内核模块，下面以最简单的hello_world内核模块程序为例。
```
root@ubuntu:/mnt/hello_world# make
make -C /lib/modules/5.0.0+/build M=/mnt/hello_world modules;
make[1]: Entering directory '/usr/src/linux'
  CC [M]  /mnt/hello_world/test-1.o
  LD [M]  /mnt/hello_world/test.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /mnt/hello_world/test.mod.o
  LD [M]  /mnt/hello_world /test.ko
make[1]: Leaving directory '/usr/src/linux'
root@ubuntu: /mnt/hello_world#
```

加载内核模块。
```
root@ubuntu:/mnt/hello_world# insmod test.ko
```
## 加入奔跑吧微信技术交流群

我们建立奔跑吧技术交流群，可以先加我们的微信号，请注明加“加奔跑交流群”：
1. 沈叔叔微信号：lcem021 （第一个字母为小写的L）
2. 笨叔微信号：runninglinuxkernel （笨叔工作忙，此微信不闲聊）
