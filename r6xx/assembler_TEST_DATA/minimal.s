# Minimal kernel, similar to the one in R6xx ISA v0.35, Table 2.4
#
# Notes:
#   No exports yet
#
# For each element in the data array compute its square
.section .alu
square:
.indexmode loop
	fmul	$0.xr, $0.xr, $0.xr
	fmul	$0.yr, $0.yr, $0.yr
	fmul	$0.zr, $0.zr, $0.zr
	fmul	$0.wr, $0.wr, $0.wr
.groupend
.type square, "func"
.size square, .-square
.section .tex
fetch:
	ld $0[xyzw]r, $127
.type fetch, "func"
.size fetch, .-fetch
.section .cf
main:
#.loopcount count
	loop_start	.L1, count
.L0:
	tex	fetch
	alu	square
	loop_end	.L0
.L1:
	nop
.programend
.type main, "func"
.size main, .-main
