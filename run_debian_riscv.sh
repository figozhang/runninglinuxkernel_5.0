#!/bin/bash

LROOT=$PWD
JOBCOUNT=${JOBCOUNT=$(nproc)}
export ARCH=riscv
export CROSS_COMPILE=riscv64-linux-gnu-
export INSTALL_PATH=$LROOT/rootfs_debian_riscv/boot/
export INSTALL_MOD_PATH=$LROOT/rootfs_debian_riscv/
export INSTALL_HDR_PATH=$LROOT/rootfs_debian_riscv/usr/

kernel_build=$PWD/rootfs_debian_riscv/usr/src/linux/

rootfs_size=2048
SMP="-smp 4"

if [ $# -lt 1 ]; then
	echo "Usage: $0 [arg]"
	echo "menuconfig: reconfig the kernel"
	echo "build_kernel: build the kernel image."
	echo "build_rootfs: build the rootfs image."
	echo "run:  run debian system."
	echo "run debug: enable gdb debug server."
fi

if [ $# -eq 2 ] && [ $2 == "debug" ]; then
	echo "Enable qemu debug server"
	DBG="-s -S"
	SMP=""
fi

make_kernel_image(){
		echo "start build kernel image..."
		make debian_defconfig
		make -j $JOBCOUNT
}

make_menuconfig(){
               echo "start re-config kernel"
               make debian_defconfig
               make menuconfig
               cp .config $PWD/arch/riscv/configs/debian_defconfig
}

prepare_rootfs(){
		if [ ! -d $LROOT/rootfs_debian_riscv ]; then
			echo "decompressing rootfs..."
			tar -Jxf rootfs_debian_riscv.tar.xz
		fi
}

build_kernel_devel(){
	kernver="$(cat include/config/kernel.release)"
	echo "kernel version: $kernver"

	mkdir -p $kernel_build
	rm rootfs_debian_riscv/lib/modules/$kernver/build
	cp -a include $kernel_build
	cp Makefile .config Module.symvers System.map vmlinux $kernel_build
	mkdir -p $kernel_build/arch/riscv/
	mkdir -p $kernel_build/arch/riscv/kernel/

	cp -a arch/riscv/include $kernel_build/arch/riscv/
	cp -a arch/riscv/Makefile $kernel_build/arch/riscv/
	cp arch/riscv/kernel/module.lds $kernel_build/arch/riscv/kernel/

	ln -s /usr/src/linux rootfs_debian_riscv/lib/modules/$kernver/build

	# ln to debian linux-kbuild-4.19 package
	ln -s /usr/src/linux-kbuild-4.19/scripts rootfs_debian_riscv/usr/src/linux/scripts
	ln -s /usr/src/linux-kbuild-4.19/tools rootfs_debian_riscv/usr/src/linux/tools
}

build_rootfs(){
		if [ ! -f $LROOT/rootfs_debian_riscv.ext4 ]; then
			make install
			make modules_install -j $JOBCOUNT
			# make headers_install

			build_kernel_devel

			echo "making image..."
			dd if=/dev/zero of=rootfs_debian_riscv.ext4 bs=1M count=$rootfs_size
			mkfs.ext4 rootfs_debian_riscv.ext4
			mkdir -p tmpfs
			echo "copy data into rootfs..."
			mount -t ext4 rootfs_debian_riscv.ext4 tmpfs/ -o loop
			cp -af rootfs_debian_riscv/* tmpfs/
			umount tmpfs
			chmod 777 rootfs_debian_riscv.ext4
		fi

}

run_qemu_debian(){
		qemu-system-riscv64 -machine virt -m 1024 $SMP\
		-kernel riscv_fw_jump.elf -device loader,file=arch/riscv/boot/Image,addr=0x80200000\
		-nographic --append "root=/dev/vda rootfstype=ext4 rw"\
		-drive if=none,file=rootfs_debian_riscv.ext4,id=hd0 -device virtio-blk-device,drive=hd0\
		-device virtio-net-device,netdev=usernet -netdev user,id=usernet\
		--fsdev local,id=kmod_dev,path=./kmodules,security_model=none\
		-device virtio-9p-device,fsdev=kmod_dev,mount_tag=kmod_mount
}

case $1 in
	build_kernel)
		make_kernel_image
		#prepare_rootfs
		#build_rootfs
		;;

	menuconfig)
		make_menuconfig
		;;
	
	build_rootfs)
		#make_kernel_image
		#check_root
		prepare_rootfs
		build_rootfs
		;;
	run)

		if [ ! -f $LROOT/arch/riscv/boot/Image ]; then
			echo "canot find kernel image, pls run build_kernel command firstly!!"
			exit 1
		fi

		if [ ! -f $rootfs_image ]; then
			echo "canot find rootfs image, pls run build_rootfs command firstly!!"
			exit 1
		fi

		#prepare_rootfs
		#build_rootfs
		run_qemu_debian
		;;
esac
