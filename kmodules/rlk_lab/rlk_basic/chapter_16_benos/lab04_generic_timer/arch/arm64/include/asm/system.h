#include <type.h>

struct pt_regs {
	struct {
		u64 regs[31];
		u64 sp;
		u64 pc;
		u64 pstate;
	};

	u64 orig_x0;
	u32 syscallno;
	u32 unused2;

	u64 orig_addr_limit;
	u64 unused;
	u64 stackframe[2];
};
