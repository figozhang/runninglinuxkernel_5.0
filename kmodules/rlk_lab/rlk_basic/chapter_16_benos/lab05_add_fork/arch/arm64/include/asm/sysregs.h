#ifndef _SYSREGS_H
#define _SYSREGS_H

#include <type.h>

/*
 * SCTLR_EL1, System Control Register (EL1)
 * AArch64-Reference-Manual v8.6, D13.2.113
 */
/* Common SCTLR_ELx flags. */
#define SCTLR_ELx_DSSBS	(_BITUL(44))
#define SCTLR_ELx_ENIA	(_BITUL(31))
#define SCTLR_ELx_ENIB	(_BITUL(30))
#define SCTLR_ELx_ENDA	(_BITUL(27))
#define SCTLR_ELx_EE    (_BITUL(25))
#define SCTLR_ELx_IESB	(_BITUL(21))
#define SCTLR_ELx_WXN	(_BITUL(19))
#define SCTLR_ELx_ENDB	(_BITUL(13))
#define SCTLR_ELx_I	(_BITUL(12))
#define SCTLR_ELx_SA	(_BITUL(3))
#define SCTLR_ELx_C	(_BITUL(2))
#define SCTLR_ELx_A	(_BITUL(1))
#define SCTLR_ELx_M	(_BITUL(0))

#define SCTLR_ELx_FLAGS	(SCTLR_ELx_M  | SCTLR_ELx_A | SCTLR_ELx_C | \
			 SCTLR_ELx_SA | SCTLR_ELx_I | SCTLR_ELx_IESB)

/* SCTLR_EL2 specific flags. */
#define SCTLR_EL2_RES1 \
	((_BITUL(4))  | (_BITUL(5))  | (_BITUL(11)) | (_BITUL(16)) | \
	(_BITUL(18)) | (_BITUL(22)) | (_BITUL(23)) | (_BITUL(28)) | \
	(_BITUL(29)))
#define SCTLR_EL2_RES0 \
	((_BITUL(6))  | (_BITUL(7))  | (_BITUL(8))  | (_BITUL(9))  | \
	(_BITUL(10)) | (_BITUL(13)) | (_BITUL(14)) | (_BITUL(15)) | \
	(_BITUL(17)) | (_BITUL(20)) | (_BITUL(24)) | (_BITUL(26)) | \
	(_BITUL(27)) | (_BITUL(30)) | (_BITUL(31)) | \
	(0xffffefffUL << 32))

#ifdef CONFIG_CPU_BIG_ENDIAN
#define ENDIAN_SET_EL2		SCTLR_ELx_EE
#define ENDIAN_CLEAR_EL2	0
#else
#define ENDIAN_SET_EL2		0
#define ENDIAN_CLEAR_EL2	SCTLR_ELx_EE
#endif

/* SCTLR_EL2 value used for the hyp-stub */
#define SCTLR_EL2_SET	(SCTLR_ELx_IESB   | ENDIAN_SET_EL2   | SCTLR_EL2_RES1)
#define SCTLR_EL2_CLEAR	(SCTLR_ELx_M      | SCTLR_ELx_A    | SCTLR_ELx_C   | \
			 SCTLR_ELx_SA     | SCTLR_ELx_I    | SCTLR_ELx_WXN | \
			 SCTLR_ELx_DSSBS | ENDIAN_CLEAR_EL2 | SCTLR_EL2_RES0)

#define SCTLR_EL2_VALUE_MMU_DISABLED (SCTLR_EL2_RES1 | ENDIAN_SET_EL2)

/* SCTLR_EL1 specific flags. */
#define SCTLR_EL1_UCI		(_BITUL(26))
#define SCTLR_EL1_E0E		(_BITUL(24))
#define SCTLR_EL1_SPAN		(_BITUL(23))
#define SCTLR_EL1_NTWE		(_BITUL(18))
#define SCTLR_EL1_NTWI		(_BITUL(16))
#define SCTLR_EL1_UCT		(_BITUL(15))
#define SCTLR_EL1_DZE		(_BITUL(14))
#define SCTLR_EL1_UMA		(_BITUL(9))
#define SCTLR_EL1_SED		(_BITUL(8))
#define SCTLR_EL1_ITD		(_BITUL(7))
#define SCTLR_EL1_CP15BEN	(_BITUL(5))
#define SCTLR_EL1_SA0		(_BITUL(4))

#define SCTLR_EL1_RES1 \
	((_BITUL(11)) | (_BITUL(20)) | (_BITUL(22)) | (_BITUL(28)) | \
	(_BITUL(29)))
#define SCTLR_EL1_RES0 \
	((_BITUL(6))  | (_BITUL(10)) | (_BITUL(13)) | (_BITUL(17)) | \
	(_BITUL(27)) | (_BITUL(30)) | (_BITUL(31)) | \
	(0xffffefffUL << 32))

#ifdef CONFIG_CPU_BIG_ENDIAN
#define ENDIAN_SET_EL1		(SCTLR_EL1_E0E | SCTLR_ELx_EE)
#define ENDIAN_CLEAR_EL1	0
#else
#define ENDIAN_SET_EL1		0
#define ENDIAN_CLEAR_EL1	(SCTLR_EL1_E0E | SCTLR_ELx_EE)
#endif

#define SCTLR_EL1_SET	(SCTLR_ELx_M    | SCTLR_ELx_C    | SCTLR_ELx_SA   |\
			 SCTLR_EL1_SA0  | SCTLR_EL1_SED  | SCTLR_ELx_I    |\
			 SCTLR_EL1_DZE  | SCTLR_EL1_UCT                   |\
			 SCTLR_EL1_NTWE | SCTLR_ELx_IESB | SCTLR_EL1_SPAN |\
			 ENDIAN_SET_EL1 | SCTLR_EL1_UCI  | SCTLR_EL1_RES1)
#define SCTLR_EL1_CLEAR	(SCTLR_ELx_A   | SCTLR_EL1_CP15BEN | SCTLR_EL1_ITD    |\
			 SCTLR_EL1_UMA | SCTLR_ELx_WXN     | ENDIAN_CLEAR_EL1 |\
			 SCTLR_ELx_DSSBS | SCTLR_EL1_NTWI  | SCTLR_EL1_RES0)

#define SCTLR_EL1_VALUE_MMU_DISABLED (SCTLR_EL1_RES1 | ENDIAN_SET_EL1)

/*
 * HCR_EL2, Hypervisor Configuration Register (EL2)
 * AArch64-Reference-Manual v8.6, D13.2.47
 */
#define HCR_FWB		(UL(1) << 46)
#define HCR_API		(UL(1) << 41)
#define HCR_APK		(UL(1) << 40)
#define HCR_TEA		(UL(1) << 37)
#define HCR_TERR	(UL(1) << 36)
#define HCR_TLOR	(UL(1) << 35)
#define HCR_E2H		(UL(1) << 34)
#define HCR_ID		(UL(1) << 33)
#define HCR_CD		(UL(1) << 32)
#define HCR_RW_SHIFT	31
#define HCR_RW		(UL(1) << HCR_RW_SHIFT)
#define HCR_TRVM	(UL(1) << 30)
#define HCR_HCD		(UL(1) << 29)
#define HCR_TDZ		(UL(1) << 28)
#define HCR_TGE		(UL(1) << 27)
#define HCR_TVM		(UL(1) << 26)
#define HCR_TTLB	(UL(1) << 25)
#define HCR_TPU		(UL(1) << 24)
#define HCR_TPC		(UL(1) << 23)
#define HCR_TSW		(UL(1) << 22)
#define HCR_TAC		(UL(1) << 21)
#define HCR_TIDCP	(UL(1) << 20)
#define HCR_TSC		(UL(1) << 19)
#define HCR_TID3	(UL(1) << 18)
#define HCR_TID2	(UL(1) << 17)
#define HCR_TID1	(UL(1) << 16)
#define HCR_TID0	(UL(1) << 15)
#define HCR_TWE		(UL(1) << 14)
#define HCR_TWI		(UL(1) << 13)
#define HCR_DC		(UL(1) << 12)
#define HCR_BSU		(3 << 10)
#define HCR_BSU_IS	(UL(1) << 10)
#define HCR_FB		(UL(1) << 9)
#define HCR_VSE		(UL(1) << 8)
#define HCR_VI		(UL(1) << 7)
#define HCR_VF		(UL(1) << 6)
#define HCR_AMO		(UL(1) << 5)
#define HCR_IMO		(UL(1) << 4)
#define HCR_FMO		(UL(1) << 3)
#define HCR_PTW		(UL(1) << 2)
#define HCR_SWIO	(UL(1) << 1)
#define HCR_VM		(UL(1) << 0)

/*
 * The bits we set in HCR:
 * TLOR:	Trap LORegion register accesses
 * RW:		64bit by default, can be overridden for 32bit VMs
 * TAC:		Trap ACTLR
 * TSC:		Trap SMC
 * TVM:		Trap VM ops (until M+C set in SCTLR_EL1)
 * TSW:		Trap cache operations by set/way
 * TWE:		Trap WFE
 * TWI:		Trap WFI
 * TIDCP:	Trap L2CTLR/L2ECTLR
 * BSU_IS:	Upgrade barriers to the inner shareable domain
 * FB:		Force broadcast of all maintenance operations
 * AMO:		Override CPSR.A and enable signaling with VA
 * IMO:		Override CPSR.I and enable signaling with VI
 * FMO:		Override CPSR.F and enable signaling with VF
 * SWIO:	Turn set/way invalidates into set/way clean+invalidate
 */
#define HCR_GUEST_FLAGS (HCR_TSC | HCR_TSW | HCR_TWE | HCR_TWI | HCR_VM | \
			 HCR_TVM | HCR_BSU_IS | HCR_FB | HCR_TAC | \
			 HCR_AMO | HCR_SWIO | HCR_TIDCP | HCR_RW | HCR_TLOR | \
			 HCR_FMO | HCR_IMO)
#define HCR_VIRT_EXCP_MASK (HCR_VSE | HCR_VI | HCR_VF)
#define HCR_HOST_NVHE_FLAGS (HCR_RW | HCR_API | HCR_APK)
#define HCR_HOST_VHE_FLAGS (HCR_RW | HCR_TGE | HCR_E2H)

/*
 * SCR_EL3, Secure Configuration Register (EL3), Page 2648 of
 * AArch64-Reference-Manual.
 */

#define SCR_RESERVED (3 << 4)
#define SCR_RW (1 << 10)
#define SCR_NS (1 << 0)
#define SCR_VALUE (SCR_RESERVED | SCR_RW | SCR_NS)

/*
 * SPSR_EL3, Saved Program Status Register (EL3) Page 389 of
 * AArch64-Reference-Manual.
 */
#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h (5 << 0)
#define SPSR_EL2h (9 << 0)
#define SPSR_EL1 (SPSR_MASK_ALL | SPSR_EL1h)
#define SPSR_EL2 (SPSR_MASK_ALL | SPSR_EL2h)

/* Current Exception Level values, as contained in CurrentEL */
#define CurrentEL_EL1 (1 << 2)
#define CurrentEL_EL2 (2 << 2)
#define CurrentEL_EL3 (3 << 2)

/*
 * 在带参数的宏，#号作为一个预处理运算符,
 * 可以把记号转换成字符串
 *
 * 下面这句话会在预编译阶段变成：
 *  asm volatile("mrs %0, " "reg" : "=r" (__val)); __val; });
 */
#define read_sysreg(reg) ({ \
		u64 _val; \
		asm volatile("mrs %0," #reg \
		: "=r"(_val)); \
		_val; \
})

#define write_sysreg(val, reg) (do { \
		u64 _val = (u64)val; \
		asm volatile("msr " #reg ", %0" \
		:: "rZ"(_val)); \
} while (0))

#define get_currentel() ({ \
		u64 _val; \
		asm volatile("mrs %0, CurrentEL" \
		: "=r"(_val)); \
		_val; \
})

#endif
