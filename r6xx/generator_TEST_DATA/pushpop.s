# Using .pushsection
main:
.pushsection .alu
# Comment in section .alu
start:
	add $1, $1, $2
.popsection
	alu @start
