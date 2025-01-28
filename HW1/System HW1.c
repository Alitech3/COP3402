/*============================================================================
| Assignment: pa01 - Encrypting a plaintext file using the Hill cipher
|
| Author: Gabriel Flores
| Language: C
| To Compile: gcc -o pa01 pa01.c
| To Execute: ./pa01 in.txt out.txt
| where kX.txt is the keytext file
| and pX.txt is plaintext file
| Note:
|
| Class: Systems Software
| Instructor:
| Due Date: 1/21
+===========================================================================*/ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 500
#define TEXT_START 10

// CPU Registers
int BP = 499, SP = 500, PC = TEXT_START;
int IR_OP = 0, IR_L = 0, IR_M = 0;

int pas[ARRAY_SIZE] = {0}; // Process Address Space (PAS) initialized to 0


int base( int BP, int L)
{
	int arb = BP;	// arb = activation record base
	while ( L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

int main(int argc, char *argv[]){

FILE *file = fopen(argv[1], "r");
while (fscanf(file, "%d %d %d", &pas[address], &pas[address + 1], &pas[address + 2]) != EOF) {
    address += 3;
}
fclose(file);
while(1){
     // Fetch Cycle:
    IR[0] = pas[PC];      // OP
    IR[1] = pas[PC + 1];  // L
    IR[2] = pas[PC + 2];  // M
    PC += 3;              // Increment PC for next instruction
    switch (IR[0]) {
    	case 1: 
	    IR[BP] = IR[2];
    	//Literal push: sp  sp- 1; pas[sp] n 
    	case 2 switch (IR[1]) {
		case 1		
	// 02 RTN 0 0 Returns from a subroutine is encoded 0 0 0 and restores the caller’s AR:
	//sp ← bp + 1; bp ← pas[sp - 2];  pc ← pas[sp - 3];

	}
	case 3 //Load value to top of stack from the stack location at offset o from n lexicographical levels down
	//sp  sp - 1;
	//pas[sp]  pas[base(bp, n) - o];
	
	case 4
	case 5 
	case 6
	case 7
	case 8
	case 9
		}
}


return 0;
}
