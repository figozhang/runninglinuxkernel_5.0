#
# arch/arm/boot/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2009 Lv Zheng
#

benos := benos.bin

quiet_cmd_strip     = STRIP   $@
      cmd_strip     = $(STRIP) -R .comment -R .note $< -o $@

benos.strip: benos FORCE
	$(call if_changed,strip)

$(obj)/benos.bin: benos.strip FORCE
	$(call if_changed,objcopy)
	@echo '  Kernel: $@ is ready'
