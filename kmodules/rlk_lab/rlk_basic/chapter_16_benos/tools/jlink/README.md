#Raspi3 Raspi4 jtag debug

1. Connect the JTAG to RPI

40-pin gpio connect

gpio22-27 Alternative Function Assignments all are ALT4.

connect following gpio pin to jlink pin.

|管脚名字|gpio功能ALT4|
|------|------|
|GPIO22 | ARM_TRST |
|GPIO23 | ARM_RTCK |
|GPIO24 | ARM_TDO |
|GPIO25 | ARM_TCK |
|GPIO26 | ARM_TDI |
|GPIO27 | ARM_TMS |
|3.3v   | Vref |
|GPIO09 | GND |

2. copy some files into SD card

Copy the config.txt and loop.bin into SD boot partition.
Insert the SD card to PI.
Power on the PI.

Connect the JLINK to PC.

3. Download the openocd.
https://github.com/xpack-dev-tools/openocd-xpack/releases

you can download the pre-build or download the source code build by youself.


4. run the openocd to connect the JLINK
$ sudo ./openocd/0.10.0-13/bin/openocd -f jlink.cfg -f raspi4.cfg

5. Open another terminal.
$ telnet localhost 4444
> halt
> load_image /home/kylin/rlk/BenOS/arch/arm64/boot/benos.bin 0x80000
> step 0x80000

6. Open another terminal.
$ gdb-multiarch --tui benos
(gdb) target remote localhost:3333
(gdb) b print_el  #set break point
(gdb) s    # step to debug
