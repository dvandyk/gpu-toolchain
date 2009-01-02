# Using .section
.section .alu
# Comment in section .alu
start:
	fadd $1.x, $1.y, $2.z
.section .cf
main:
	alu @start
