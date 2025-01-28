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

int pas[ARRAY_SIZE] = {0}; // Process Address Space (PAS) initialized to 0
int BP = 499, SP = 500, PC = 10; // Initial values for the registers
int IR[3]; // Instruction Register (holds OP, L, M)

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
    switch IR[0]
    case 1
}


return 0;
}