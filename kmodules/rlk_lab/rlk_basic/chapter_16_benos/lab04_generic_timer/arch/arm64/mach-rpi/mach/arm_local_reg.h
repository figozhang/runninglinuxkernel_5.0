#include "base.h"

/*
 * ARM Local register for PI
 * see <BCM2711 ARM Peripherals> 6.5.2
 */

#define ARM_CONTROL (ARM_LOCAL_BASE + 0x0)
#define CORE_IRQ_CONTROL (ARM_LOCAL_BASE + 0xc)
#define PMU_CONTROL_SET (ARM_LOCAL_BASE + 0x10)
#define PMU_CONTROL_CLR (ARM_LOCAL_BASE + 0x14)
#define PERI_IRQ_ROUTE0 (ARM_LOCAL_BASE + 0x24)
#define AXI_QUIET_TIME (ARM_LOCAL_BASE + 0x30)
#define LOCAL_TIMER_CONTROL (ARM_LOCAL_BASE + 0x34)
#define LOCAL_TIMER_IRQ (ARM_LOCAL_BASE + 0x38)

#define TIMER_CNTRL0 (ARM_LOCAL_BASE + 0x40)
#define TIMER_CNTRL1 (ARM_LOCAL_BASE + 0x44)
#define TIMER_CNTRL2 (ARM_LOCAL_BASE + 0x48)
#define TIMER_CNTRL3 (ARM_LOCAL_BASE + 0x4c)
/* Secure Physical Timer Event for IRQ */
#define CNT_PS_IRQ (1 << 0)
/* Nonsecure Physical Timer Event for IRQ */
#define CNT_PNS_IRQ (1 << 1)
/* Hypervisor Physical Timer Event for IRQ */
#define CNT_HP_IRQ (1 << 2)
/* Virtual Timer Event for IRQ */
#define CNT_V_IRQ (1 << 3)
/* Secure Physical Timer Event for FIQ */
#define CNT_PS_IRQ_FIQ (1 << 4)
/* Nonsecure Physical Timer Event for FIQ */
#define CNT_PNS_IRQ_FIQ (1 << 5)
/* Hypervisor Physical Timer Event for FIQ */
#define CNT_HP_IRQ_FIQ (1 << 6)
/* Virtual Timer Event for FIQ */
#define CNT_V_IRQ_FIQ (1 << 7)

#define ARM_LOCAL_IRQ_SOURCE0 (ARM_LOCAL_BASE + 0x60)
#define ARM_LOCAL_IRQ_SOURCE1 (ARM_LOCAL_BASE + 0x64)
#define ARM_LOCAL_IRQ_SOURCE2 (ARM_LOCAL_BASE + 0x68)
#define ARM_LOCAL_IRQ_SOURCE3 (ARM_LOCAL_BASE + 0x6c)
#define MAILBOX_IRQ_SHIFT 4
#define CORE_IRQ (1 << 8)
#define PMU_IRQ (1 << 9)
#define AXI_QUIET (1 << 10)
#define TIMER_IRQ (1 << 11)
#define AXI_IRQ (1 << 30)

#define ARM_LOCAL_FRQ_SOURCE0 (ARM_LOCAL_BASE + 0x70)
#define ARM_LOCAL_FRQ_SOURCE1 (ARM_LOCAL_BASE + 0x74)
#define ARM_LOCAL_FRQ_SOURCE2 (ARM_LOCAL_BASE + 0x78)
#define ARM_LOCAL_FRQ_SOURCE3 (ARM_LOCAL_BASE + 0x7c)
