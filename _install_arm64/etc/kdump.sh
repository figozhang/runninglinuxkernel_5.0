#!/bin/sh

KDUMP_PATH="/var/crash"
MYDATA=`date +%Y-%m-%d-%H-%M-%S`
coredir=$KDUMP_PATH/127.0.0.1.${MYDATA}

save_dump_file()
{
	echo "path is $coredir"
	mkdir -p $coredir
	if [ ! -f /sbin/vmcore-dmesg ];then
		echo "skipping save vmcore"
		return;
	fi

	echo "kdump: save vmcore-dmesg.txt"
	/sbin/vmcore-dmesg /proc/vmcore > $coredir/vmcore-dmesg.txt
	cp /proc/vmcore $coredir
}

if [ -s /proc/vmcore ];then
	echo "saving vmcore now..."
	save_dump_file
	reboot
else
	# kexec setting
	kexec -p --command-line="root=/dev/vda rw" /boot/Image
fi
