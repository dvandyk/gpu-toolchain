# Minimal kernel, similar to the one in R6xx ISA v0.35, Table 2.4
#
# Notes:
#   No exports yet
#
# For each element in the data array compute its square
.section .alu
square:
.indexmode loop
	fmul	$0.x, $0.x, $0.x
	fmul	$0.y, $0.y, $0.y
	fmul	$0.z, $0.z, $0.z
	fmul	$0.w, $0.w, $0.w
.groupend
.type square, "func"
.size square, .-square
.section .tex
fetch:
	ld $0[xyzw], $127
.type fetch, "func"
.size fetch, .-fetch
.section .cf
main:
	loop_start	.L1, loopcounter
.L0:
	tex	fetch
	alu	square
	loop_end	.L0
.L1:
	nop
.programend
.type main, "func"
.size main, .-main
.section .gpgpu.notes
