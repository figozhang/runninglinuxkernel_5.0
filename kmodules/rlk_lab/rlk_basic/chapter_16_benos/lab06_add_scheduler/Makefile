VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
NAME = benshushu

MAKEFLAGS += -rR --no-print-directory

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

KBUILD_CHECKSRC = 0

# Beautify output
ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

export quiet Q KBUILD_VERBOSE

# We process the rest of the Makefile if this is the final invocation of make
ifeq ($(skip-makefile),)

# If building an external module we do not care about the all: rule
# but instead _all depend on modules
PHONY += all
_all: all

srctree		:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
TOPDIR		:= $(srctree)
# FIXME - TOPDIR is obsolete, use srctree/objtree
objtree		:= $(CURDIR)
src		:= $(srctree)
obj		:= $(objtree)

VPATH		:= $(srctree)

export srctree objtree VPATH TOPDIR


# SUBARCH tells the usermode build what the underlying arch is.

SUBARCH := arm64
SUBARCH_CROSS_COMPILE := aarch64-linux-gnu-

ARCH		?= $(SUBARCH)
CROSS_COMPILE	?= $(SUBARCH_CROSS_COMPILE)

# Architecture as present in compile.h
SRCARCH         := $(ARCH)
UTS_MACHINE := $(ARCH)

KCONFIG_CONFIG	?= .config

# SHELL used by kbuild
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

HOSTCC       = gcc
HOSTCXX      = g++
HOSTINCLUDE  = -Iinclude
HOSTCFLAGS   = -w -O2 -fomit-frame-pointer $(HOSTINCLUDE)
HOSTCXXFLAGS = -O2

# Decide whether to build built-in, modular, or both.
# Normally, just do built-in.

KBUILD_BUILTIN := 1

export KBUILD_BUILTIN
export KBUILD_CHECKSRC KBUILD_SRC


hdr-arch  := $(SRCARCH)

# Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(srctree)

# We need some generic definitions.
#include $(srctree)/arch/$(ARCH)/Kbuild.include
include $(srctree)/scripts/Kbuild.include

# Use TARGETINCLUDE when you must reference the include/ directory.
# Needed to be compatible with the O= option
TARGETINCLUDE   := -Iinclude -Iinclude/std \
                   -include $(srctree)/include/target/config.h \
                   $(if $(KBUILD_SRC),-Iinclude2 -I$(srctree)/include)

# Use USERINCLUDE when you must reference the UAPI directories only.
USERINCLUDE    := \
		-I$(srctree)/arch/$(hdr-arch)/include/uapi \
		-Iarch/$(hdr-arch)/include/generated/uapi \
		-I$(srctree)/include/uapi \
		-Iinclude/generated/uapi

# Use LINUXINCLUDE when you must reference the include/ directory.
# Needed to be compatible with the O= option
LINUXINCLUDE    := \
		-I$(srctree)/arch/$(hdr-arch)/include \
		-Iarch/$(hdr-arch)/include/generated/uapi \
		-Iarch/$(hdr-arch)/include/generated \
		-Iarch/$(hdr-arch)/include/asm \
		$(if $(KBUILD_SRC), -I$(srctree)/include) \
		-Iinclude \
		$(USERINCLUDE)

export LINUXINCLUDE

# Make variables (CC, etc...)

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CROSS_COMPILE)cpp
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
CPPFLAGS        := -g -D__KERNEL__ $(TARGETINCLUDE) $(LINUXINCLUDE)
CFLAGS          := -g -Wall -DKBUILD_CONF $(TARGETINCLUDE) $(LINUXINCLUDE)
AFLAGS          := -D__ASSEMBLY__
SYMLINK		:= ln -fsn
#SYMLINK		:= cp -rf
AWK		= awk
PERL		= perl
CHECK		= sparse

CHECKFLAGS     := -D__STDC__ $(CF)
CFLAGS_KERNEL	=
AFLAGS_KERNEL	=

# Read KERNELRELEASE from include/config/kernel.release (if it exists)
KERNELRELEASE = $(shell cat include/config/kernel.release 2> /dev/null)
KERNELVERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

export VERSION PATCHLEVEL SUBLEVEL KERNELRELEASE KERNELVERSION
export ARCH  SRCARCH CONFIG_SHELL HOSTCC HOSTCFLAGS HOSTINCLUDE CROSS_COMPILE
export AS LD CC CPP AR NM STRIP OBJCOPY MAKE AWK PERL
export UTS_MACHINE HOSTCXX HOSTCXXFLAGS CHECK CHECKFLAGS

export CPPFLAGS NOSTDINC_FLAGS TARGETINCLUDE OBJCOPYFLAGS LDFLAGS
export CFLAGS CFLAGS_KERNEL
export AFLAGS AFLAGS_KERNEL

# Files to ignore in find ... statements

RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS -o -name .pc -o -name .hg -o -name .git \) -prune -o
export RCS_TAR_IGNORE := --exclude SCCS --exclude BitKeeper --exclude .svn --exclude CVS --exclude .pc --exclude .hg --exclude .git

# ===========================================================================
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

scripts/basic/%: scripts_basic ;

PHONY += outputmakefile
# outputmakefile generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of make in the
# output directory.
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# To make sure we do not include .config for any of the *config targets
# catch them early, and hand them over to scripts/kconfig/Makefile
# It is allowed to specify more targets when calling make, including
# mixing *config targets and build targets.
# For example 'make oldconfig all'.
# Detect when mixed targets is specified, and make a second invocation
# of make so .config is not included in this case either (for *config).

no-dot-config-targets := clean mrproper distclean \
			 cscope TAGS tags help %docs check% \
			 include/target/version.h headers_% \
			 kernelrelease kernelversion

config-targets := 0
mixed-targets  := 0
dot-config     := 1

ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
	ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
		dot-config := 0
	endif
endif

ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(filter-out config %config,$(MAKECMDGOALS)),)
                mixed-targets := 1
        endif
endif

ifeq ($(mixed-targets),1)
# ===========================================================================
# We're called with mixed targets (*config and build targets).
# Handle them one by one.

%:: FORCE
	$(Q)$(MAKE) -C $(srctree) KBUILD_SRC= $@

else
ifeq ($(config-targets),1)
# ===========================================================================
# *config targets only - make sure prerequisites are updated, and descend
# in scripts/kconfig to make the *config target

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration
# used for 'make defconfig'
include $(srctree)/arch/$(ARCH)/Makefile
export KBUILD_DEFCONFIG

config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p include/target include/config
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)mkdir -p include/target include/config
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else
# ===========================================================================
# Build targets only - this includes kernel, arch specific targets, clean
# targets and others. In general all targets except *config targets.

# Additional helpers built in scripts/
# Carefully list dependencies so we do not try to build scripts twice
# in parallel
PHONY += scripts
scripts: scripts_basic include/config/auto.conf
	$(Q)$(MAKE) $(build)=$(@)

# Objects we will link into kernel / subdirs we need to visit
init-y		:= init/
libs-y		:= lib/
#drivers-y	:= drivers/
core-y		:= kernel/
core-y          += mm/

ifeq ($(dot-config),1)
# Read in config
-include include/config/auto.conf

# Read in dependencies to all Kconfig* files, make sure to run
# oldconfig if changes are detected.
-include include/config/auto.conf.cmd

# To avoid any implicit rule to kick in, define an empty command
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

# If .config is newer than include/config/auto.conf, someone tinkered
# with it and forgot to run make oldconfig.
# if auto.conf.cmd is missing then we are probably in a cleaned tree so
# we execute the config step to be sure to catch updated Kconfig files
include/config/auto.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig

else
# Dummy target needed, because used as prerequisite
include/config/auto.conf: ;
endif # $(dot-config)

# The all: target is the default when no target is given on the
# command line.
# This allow a user to issue only 'make' to build a kernel
# Defaults benos but it is usually overridden in the arch makefile
all: benos

include $(srctree)/arch/$(ARCH)/Makefile
ifdef CONFIG_CC_OPT_SIZE
CFLAGS		+= -Os
endif
ifdef CONFIG_CC_OPT_SPEED
CFLAGS		+= -O2
endif

ifdef CONFIG_FRAME_POINTER
CFLAGS		+= -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
CFLAGS		+= -fomit-frame-pointer
endif

ifdef CONFIG_CC_GEN_DEBUG
CFLAGS		+= -g
endif

NOSTDINC_FLAGS += -nostdlib -mgeneral-regs-only -ffreestanding -nostartfiles

CHECKFLAGS     += $(NOSTDINC_FLAGS)

# Default benos image to build when no specific target is given.
# KBUILD_IMAGE may be overruled on the command line or
# set in the environment
export KBUILD_IMAGE ?= benos

#
# INSTALL_PATH specifies where to place the updated kernel and map
# images. Default is /boot, but you can set it to other values
export	INSTALL_PATH ?= /boot

core-y		+= # kernel/ mm/

benos-dirs	:= $(patsubst %/,%,$(filter %/, $(init-y) $(init-m) \
		     $(libs-y) $(libs-m) \
		     $(core-y) $(core-m) \
		     $(drivers-y) $(drivers-m)))

benos-alldirs	:= $(sort $(benos-dirs) $(patsubst %/,%,$(filter %/, \
		     $(lib-n) $(lib-) \
		     $(init-n) $(init-) \
		     $(core-n) $(core-) \
		     $(drivers-n) $(drivers-))))

init-y		:= $(patsubst %/, %/built-in.lib, $(init-y))
libs-y		:= $(patsubst %/, %/built-in.lib, $(libs-y))
core-y		:= $(patsubst %/, %/built-in.lib, $(core-y))
drivers-y	:= $(patsubst %/, %/built-in.lib, $(drivers-y))

# Build kernel
# ---------------------------------------------------------------------------
# benos is built from the objects selected by $(benos-init) and
# $(benos-main). Most are built-in.lib files from top-level directories
# in the kernel tree, others are specified in arch/$(ARCH)/Makefile.
# Ordering when linking is important, and $(benos-init) must be first.
#
# benos
#   ^
#   |
#   +-< $(benos-init)
#   |   +--< init/version.o + more
#   |
#   +--< $(benos-main)
#        +--< driver/built-in.lib mm/built-in.lib + more
#
# kernel version (uname -v) cannot be updated during normal
# descending-into-subdirs phase since we do not yet know if we need to
# update kernel.
# Therefore this step is delayed until just before final link of kernel.
#
# System.map is generated to document addresses of all kernel symbols

benos-init := $(head-y) $(init-y)
benos-main := $(core-y) $(libs-y) $(drivers-y)
benos-all  := $(benos-init) $(benos-main)
benos-lds  := arch/$(ARCH)/kernel/benos.lds
benos-map  := benos.map
export KBUILD_VMLINUX_OBJS := $(benos-all)

# Generate new kernel version
quiet_cmd_benos_version = GEN     .version
      cmd_benos_version = set -e;                        \
	if [ ! -r .version ]; then			\
	  rm -f .version;				\
	  echo 1 >.version;				\
	else						\
	  mv .version .old_version;			\
	  expr 0$$(cat .old_version) + 1 >.version;	\
	fi;						\
	$(MAKE) $(build)=init

# The finally linked kernel.
quiet_cmd_benos = LD      $@
      cmd_benos = $(LD) $(LDFLAGS) $(LDFLAGS_benos) -o $@ \
	-T $(benos-lds) -Map $(benos-map) $(benos-init) \
	--start-group $(benos-main) --end-group \
	$(filter-out $(benos-lds) $(benos-init) $(benos-main) FORCE ,$^)
define rule_benos
	:
	+$(call cmd,benos)
	$(Q)echo 'cmd_$@ := $(cmd_benos)' > $(dot-target).cmd
endef

# kernel image - including updated kernel symbols
benos: $(benos-lds) $(benos-init) $(benos-main) FORCE
	$(call if_changed_rule,benos)
	$(Q)rm -f .old_version

# The actual objects are generated when descending,
# make sure no implicit rule kicks in
$(sort $(benos-init) $(benos-main) $(benos-lds)): $(benos-dirs) ;

# Handle descending into subdirectories listed in $(benos-dirs)
# Preset locale variables to speed up the build process. Limit locale
# tweaks to this spot to avoid wrong language settings when running
# make menuconfig etc.
# Error messages still appears in the original language

PHONY += $(benos-dirs)
$(benos-dirs): prepare scripts
	$(Q)$(MAKE) $(build)=$@

# Build the kernel release string
pattern = ".*/localversion[^~]*"
string  = $(shell cat /dev/null \
	   `find $(objtree) $(srctree) -maxdepth 1 -regex $(pattern) | sort -u`)

localver = $(subst $(space),, $(string) \
			      $(patsubst "%",%,$(CONFIG_LOCALVERSION)))

# If CONFIG_LOCALVERSION_AUTO is set scripts/setlocalversion is called
# and if the SCM is know a tag from the SCM is appended.
# The appended tag is determined by the SCM used.
#
# Currently, only git is supported.
# Other SCMs can edit scripts/setlocalversion and add the appropriate
# checks as needed.
ifdef CONFIG_LOCALVERSION_AUTO
	_localver-auto = $(shell $(CONFIG_SHELL) \
	                  $(srctree)/scripts/setlocalversion $(srctree))
	localver-auto  = $(LOCALVERSION)$(_localver-auto)
endif

localver-full = $(localver)$(localver-auto)

# Store (new) KERNELRELASE string in include/config/kernel.release
kernelrelease = $(KERNELVERSION)$(localver-full)
include/config/kernel.release: include/config/auto.conf FORCE
	$(Q)rm -f $@
	$(Q)echo $(kernelrelease) > $@

# Listed in dependency order
PHONY += prepare archprepare prepare0 prepare1 prepare2 prepare3

# prepare3 is used to check if we are building in a separate output directory,
# and if so do:
# 1) Check that make has not been executed in the kernel src $(srctree)
# 2) Create the include2 directory, used for the second asm symlink
prepare3: include/config/kernel.release
ifneq ($(KBUILD_SRC),)
	@echo '  Using $(srctree) as source for kernel'
	$(Q)if [ -f $(srctree)/.config -o -d $(srctree)/include/config ]; then \
		echo "  $(srctree) is not clean, please run 'make mrproper'";\
		echo "  in the '$(srctree)' directory.";\
		/bin/false; \
	fi;
	$(Q)if [ ! -d include2 ]; then mkdir -p include2; fi;
	$(Q)ln -fsn $(srctree)/include/asm-$(ARCH) include2/asm
endif

# prepare2 creates a makefile if using a separate output directory
prepare2: prepare3 outputmakefile

prepare1: prepare2 include/target/version.h include/target/utsrelease.h \
                   include/asm include/config/auto.conf

archprepare: prepare1

prepare0: archprepare FORCE
	$(Q)$(MAKE) $(build)=.

# All the preparing..
prepare: prepare0

# Leave this as default for preprocessing benos.lds.S, which is now
# done in arch/$(ARCH)/kernel/Makefile

export CPPFLAGS_benos.lds += -P -C -U$(ARCH)

# FIXME: The asm symlink changes when $(ARCH) changes. That's
# hard to detect, but I suppose "make mrproper" is a good idea
# before switching between archs anyway.
# FIXME: SDCC can not recognize cygwin's symlink

include/asm:
	@echo '  SYMLINK $@ -> arch/$(ARCH)/include/asm'
	$(Q)if [ ! -d include ]; then mkdir -p include; fi;
	@$(SYMLINK) $(TOPDIR)/arch/$(ARCH)/include/asm $@

# Generate some files
# ---------------------------------------------------------------------------

# KERNELRELEASE can change from a few different places, meaning version.h
# needs to be updated, so this check is forced on all builds

uts_len := 64
define filechk_utsrelease.h
	if [ `echo -n "$(KERNELRELEASE)" | wc -c ` -gt $(uts_len) ]; then \
	  echo '"$(KERNELRELEASE)" exceeds $(uts_len) characters' >&2;    \
	  exit 1;                                                         \
	fi;                                                               \
	(echo \#define UTS_RELEASE \"$(KERNELRELEASE)\";)
endef

define filechk_version.h
	(echo \#define LINUX_VERSION_CODE $(shell                             \
	expr $(VERSION) \* 65536 + $(PATCHLEVEL) \* 256 + $(SUBLEVEL));     \
	echo '#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))';)
endef

include/target/version.h: $(srctree)/Makefile FORCE
	$(call filechk,version.h)

include/target/utsrelease.h: include/config/kernel.release FORCE
	$(call filechk,utsrelease.h)

###
# Cleaning is done on three levels.
# make clean     Delete most generated files
# make distclean Remove editor backup files, patch leftover files and the like

# Directories & files removed with 'make clean'
CLEAN_FILES +=	benos benos.strip benos.map \
                .tmp_version .tmp_benos* .tmp_benos.map
CLEAN_DIRS += include/asm

# Directories & files removed with 'make mrproper'
MRPROPER_DIRS  += include/config include2 usr/include
MRPROPER_FILES += .config .config.old .version .old_version \
                  include/target/autoconf.h include/target/version.h      \
                  include/target/utsrelease.h                            \
		  Module.symvers tags TAGS cscope*

# clean - Delete most, but leave enough to build external modules
#
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_,$(srctree) $(benos-alldirs))

PHONY += $(clean-dirs) clean archclean
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: archclean $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '*.rel' -o -name '*.sym' -o -name '*.lst' \
		-o -name '*.hex' -o -name '*.mem' -o -name '*.lnk' \
		-o -name '*.asm' -o -name '*.rst' -o -name '*.lib' \
		-o -name '*.adb' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \) \
		-type f -print | xargs rm -f

# mrproper - Delete all generated files, including .config
#
mrproper: rm-dirs  := $(wildcard $(MRPROPER_DIRS))
mrproper: rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs      := $(addprefix _mrproper_, scripts)

PHONY += $(mrproper-dirs) mrproper archmrproper
$(mrproper-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean archmrproper $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

# distclean
#
PHONY += distclean

distclean: mrproper
	@find $(srctree) $(RCS_FIND_IGNORE) \
		\( -name '*.orig' -o -name '*.rej' -o -name '*~' \
		-o -name '*.bak' -o -name '#*#' -o -name '.*.orig' \
		-o -name '.*.rej' -o -size 0 \
		-o -name '*%' -o -name '.*.cmd' -o -name 'core' \) \
		-type f -print | xargs rm -f

help:
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config and'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with [*]'
	@echo  '  kernelrelease	  - Output the release version string'
	@echo  '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*] '
	@echo  'For further info see the ./README file'

endif #ifeq ($(config-targets),1)
endif #ifeq ($(mixed-targets),1)

PHONY += kernelrelease kernelversion
kernelrelease:
	$(if $(wildcard include/config/kernel.release), $(Q)echo $(KERNELRELEASE), \
		$(error kernelrelease not valid - run 'make prepare' to update it))
kernelversion:
	@echo $(KERNELVERSION)

# FIXME Should go into a make.lib or something
# ===========================================================================

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)


a_flags = -Wp,-MD,$(depfile) $(AFLAGS) $(AFLAGS_KERNEL) \
	  $(NOSTDINC_FLAGS) $(CPPFLAGS) \
	  $(modkern_aflags) $(EXTRA_AFLAGS) $(AFLAGS_$(basetarget).o)

quiet_cmd_as_o_S = AS      $@
cmd_as_o_S       = $(CC) $(a_flags) -c -o $@ $<

# read all saved command lines

targets := $(wildcard $(sort $(targets)))
cmd_files := $(wildcard .*.cmd $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))

ifneq ($(cmd_files),)
  $(cmd_files): ;	# Do not try to update included dependency files
  include $(cmd_files)
endif

# Shorthand for $(Q)$(MAKE) -f scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.clean obj

endif	# skip-makefile

PHONY += FORCE
FORCE:

# Cancel implicit rules on top Makefile, `-rR' will apply to sub-makes.
Makefile: ;

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable se we can use it in if_changed and friends.
.PHONY: $(PHONY)
