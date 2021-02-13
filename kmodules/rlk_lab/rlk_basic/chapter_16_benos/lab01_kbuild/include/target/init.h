#ifndef __INIT_H_INCLUDE__
#define __INIT_H_INCLUDE__

#define __HEAD		.section	".head.text","ax"
#define __INIT		.section	".init.text","ax"

/* ARM: THUMB2 firmware specific entry vector */
#define __LOVEC		.section	".lovec.text","ax"

#endif
