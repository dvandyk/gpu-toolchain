# Using .pushsection
main:
.pushsection .alu
# Comment in section .alu
start:
	fadd $1.x, $1.y, $2.z
.popsection
	alu @start
