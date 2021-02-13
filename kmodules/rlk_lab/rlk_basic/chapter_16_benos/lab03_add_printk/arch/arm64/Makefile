#
# arch/arm64/Makefile
#

LDFLAGS_benos	:=-p --no-undefined -X
#CPPFLAGS_vmlinux.lds = -DTEXT_OFFSET=$(TEXT_OFFSET)
OBJCOPYFLAGS	:=-O binary -R .note -R .note.gnu.build-id -R .comment -S
GZFLAGS		:=-9

KBUILD_DEFCONFIG := defconfig

KBUILD_CFLAGS	+= -mgeneral-regs-only
ifeq ($(CONFIG_CPU_BIG_ENDIAN), y)
KBUILD_CPPFLAGS	+= -mbig-endian
AS		+= -EB
LD		+= -EB
else
KBUILD_CPPFLAGS	+= -mlittle-endian
#AS		+= -EL
#LD		+= -EL
endif

CHECKFLAGS	+= -D__aarch64__

# Default value
head-y		:= arch/arm64/kernel/head.o

# The byte offset of the kernel image in RAM from the start of RAM.
#TEXT_OFFSET := 0x00080000

export	TEXT_OFFSET GZFLAGS

core-y		+= arch/arm64/kernel/

## add mach-rpi folder
ifeq ($(CONFIG_ARCH_BCM2835),y)
machine-$(CONFIG_ARCH_BCM2835)      := rpi
incdir-$(CONFIG_ARCH_BCM2835)	:= rpi
endif

ifeq ($(incdir-y),)
incdir-y := $(machine-y)
endif
INCDIR   := mach-$(incdir-y)

ifneq ($(machine-y),)
MACHINE  := arch/arm64/mach-$(machine-y)/
endif

machdirs := $(patsubst %,arch/arm/mach-%/,$(machine-y))

core-y  += $(MACHINE)

# Default target when executing plain make
KBUILD_IMAGE	:= benos.bin

all:	$(KBUILD_IMAGE)

boot := arch/arm64/boot

include/mach:
	@echo '  SYMLINK include/asm/mach -> arch/arm64/$(INCDIR)/mach'
	@$(SYMLINK) $(TOPDIR)/arch/arm64/$(INCDIR)/mach $@

benos.bin: benos
	@$(MAKE) $(build)=$(boot) $(boot)/$@

archprepare: maketools

PHONY += maketools
maketools : include/mach

CLEAN_FILES += arch/$(ARCH)/boot/benos.bin
CLEAN_FILES += include/mach

# We use MRPROPER_FILES and CLEAN_FILES now
archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
  echo  '* Image.gz      - Compressed kernel image (arch/$(ARCH)/boot/Image.gz)'
  echo  '  Image         - Uncompressed kernel image (arch/$(ARCH)/boot/Image)'
  echo  '                  Install using (your) ~/bin/installkernel or'
  echo  '                  (distribution) /sbin/installkernel or'
  echo  '                  install to $$(INSTALL_PATH) and run lilo'
endef

ifeq ($(CONFIG_BOARD_PI3B), y)
QEMU_FLAGS  += -machine raspi3
endif

ifeq ($(CONFIG_BOARD_PI4B), y)
QEMU_FLAGS  += -machine raspi4
endif

ifeq ($(CONFIG_PI_MINI_UART),y)
QEMU_FLAGS  += -serial null -serial mon:stdio
endif

QEMU_FLAGS  += -nographic

run:
	qemu-system-aarch64 $(QEMU_FLAGS) \
	-kernel arch/arm64/boot/benos.bin

debug:
	qemu-system-aarch64 $(QEMU_FLAGS) \
	-kernel arch/arm64/boot/benos.bin -S -s
