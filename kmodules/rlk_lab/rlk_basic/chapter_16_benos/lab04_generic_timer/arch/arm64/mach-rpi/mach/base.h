#ifndef	_P_BASE_H
#define	_P_BASE_H

#ifdef CONFIG_BOARD_PI3B
#define PBASE 0x3F000000
#define ARM_LOCAL_BASE 0x40000000
#else
/* Main peripherals on Low Peripheral mode
 * - ARM and GPU can access
 * see <BCM2711 ARM Peripherals> 1.2.4
 */
#define PBASE 0xFE000000
/*
 * ARM LOCAL register on Low Peripheral mode
 * - Only ARM can access
 * see <BCM2711 ARM Peripherals> 6.5.2
 */
#define ARM_LOCAL_BASE 0xff800000
#endif

#endif  /*_P_BASE_H */
