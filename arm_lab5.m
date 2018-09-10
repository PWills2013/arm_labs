  .global main
  .func main
  
main:
	SUB SP, SP, #16
	LDR R0, .filedir            @ Get /dev/mem file address
	LDR R1, .flags          	@ Set flags for file permissions
	BL  open                    @ Call open function, R0 will have file descriptor

_map:
	STR R0, [SP, #12]           @ Save file descriptor on stack
	LDR R3, [SP, #12]           @ R3 gets a copy of the file descriptor
	STR R3, [sp, #0]            @ Store the file descriptor at the top of the stack (SP + 0)
	LDR R3, .tBase           	@ Get tbase base address in R3
	STR R3, [sp, #4]            @ Store the tbase base address in the stack (SP + 4)

    @ Parameters for mmap function, the 4 below and the file descriptor and gpio
    @ base address in the stack. This lets the kernel choose the vmem address,
    @ sets the page size, desired memory protection.
	MOV R0, #0
	MOV R1, #4096
	MOV R2, #1
	MOV R3, #1
	BL  mmap			@ R0 now has the vmem address.	

clear:
	STR R0, [SP, #16]           @ Store vmem address in stack
	LDR R3, [SP, #16]           @ Make a copy of vmem address in R3
	ADD R3, R3, #4              @ Add 4 bytes to R3 to get address of 

	LDR R6, [R3] @save first timestamp
	LSR R6, #20 @shift 1e6
	
_loopHere:
	LDR R3, [SP, #16]           @ Make a copy of vmem address in R3
	ADD R3, R3, #4              @ Add 4 bytes to R3 to get address of CLO
	LDR R1, [R3]
	LSR R1, #20 			@ shift by 1e6
	SUB R1, R1, R6		@ Convert output to number of seconds since program began
	LDR R0, =out
	CMP R1, R7
	MOV R8, R1
	BLNE printf
	MOV R7, R8
	B _loopHere
	
	@close
	LDR R0, [SP, #12]
	BL close
	ADD SP, SP, #16

	
.tBase:		.word 	0x3F003000
.filedir:	.word   .file
.flags:		.word   1576962
.file: 		.asciz "/dev/mem"
.data
	out: 	.asciz "%d \n"