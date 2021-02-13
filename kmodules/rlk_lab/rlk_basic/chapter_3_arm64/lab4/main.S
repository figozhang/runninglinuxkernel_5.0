.section .data
.align 3

print_data:
	.string "big data: %d\n"

.section .text
.globl main
main:
	stp     x29, x30, [sp, -16]!

	mov x0, #6
	mov x1, #5
	bl compare_data

	mov x1, x0
	ldr x0, =print_data
	bl printf

	ldp  x29, x30, [sp], 16
	ret

	

