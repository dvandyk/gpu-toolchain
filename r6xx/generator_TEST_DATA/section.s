# Using .section
.section .alu
# Comment in section .alu
start:
	add $1, $1, $2
.section .cf
main:
	alu @start
