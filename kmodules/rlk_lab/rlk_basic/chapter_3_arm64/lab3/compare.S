
.section .text
.globl compare_data
compare_data:
	cmp x0, x1
	csel x0, x0, x1, hi
	ret
