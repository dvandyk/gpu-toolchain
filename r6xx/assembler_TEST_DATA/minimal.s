# Minimal kernel, similar to the one in R6xx ISA v0.35, Table 2.4
#
# For each element in the data array compute its square
.section .alu
square:
	fmul	$0.x, $0.x, $0.x
	fmul	$0.y, $0.y, $0.y
	fmul	$0.z, $0.z, $0.z
	fmul	$0.w, $0.w, $0.w
.groupend
.type square, "func"
.size square, .-square
.section .tex
fetch:
	ld $0[xyzw], $0 #FIXME: should be 'input'
.type fetch, "func"
.size fetch, .-fetch
.section .cf
main:
	tex	fetch
	alu	square
#FIXME	export	output, $0[xyzw]
	nop
.programend
.type main, "func"
.size main, .-main
.section .gpgpu.notes
.section .gpgpu.data
.buffer input
.buffer output
