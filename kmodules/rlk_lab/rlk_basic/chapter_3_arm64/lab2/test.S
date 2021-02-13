.section .data
.align 3
my_data:
	.quad  1
	.quad  2
	.quad  5
	.quad  8
	.quad  10
	.quad  12

my_data_count:
	.quad  6

.align	3
print_data:
	.string "big data: %d\n"

.section .text
.globl main
main:
	stp     x29, x30, [sp, -16]!

	ldr x0, =my_data
	ldr x1, my_data_count

	add x4, x0, #40

	mov x3, xzr
1:
	ldr x2, [x0], #8
	cmp x2, x3
	csel x3, x2, x3, hi

	cmp x0, x4
	b.ls 1b

	ldr x0, =print_data
	//adrp x0, print_data
	//add x0, x0, :lo12:print_data
	mov x1, x3

	bl printf

	ldp  x29, x30, [sp], 16
	ret
