Spectre CVE-2017-5753 PoC for Android/Linux/ARM64

Original Spectre PoC code is from appendix in its paper https://spectreattack.com/spectre.pdf

I've been testing alternative solutions to replace the X86 cache flush and timer instructions, not very stable. 

Now it become stable, thanks to methods from @lgeek https://github.com/lgeek/spec_poc_arm/blob/master/dump_sys_regs.c

with a standard Debian AArch64 toolchain:
#aarch64-linux-gnu-gcc source.c -pie -std=gnu99 -lpthread
