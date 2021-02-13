#ifndef __CONFIG_H_INCLUDE__
#define __CONFIG_H_INCLUDE__

#include <target/autoconf.h>

/* Three system performance ? should be considered carefully to choose the
 * software architecture model:
 * 1. throughput: IO throughput should be as great as possible
 * 2. realtime: time critical tasks can be run on time
 * 3. faireness: tasks can be distributed fairly on difference computing
 *               resources
 * Systems will always take throughput into account, realtime and fairness
 * are left for users' choices.
 */
#if defined(CONFIG_SYS_NOIRQ)
#define SYS_BOOTLOAD 1 /* run on non-VIC system to boot something */
#endif
#if defined(CONFIG_SYS_NOIRQ) || defined(CONFIG_SYS_POLL_RT)
#define SYS_REALTIME 1 /* realtime work is the most priority thing */
#endif
#if defined(CONFIG_SYS_TASK)
#define SYS_FAIRNESS 1 /* task fairness is the most priority thing */
#endif

#endif /* __CONFIG_H_INCLUDE__ */
