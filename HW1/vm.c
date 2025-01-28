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
int IR[3];
int EOP = 1;

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

	printf("%s", "Virtual Machine\n");

FILE *file = fopen(argv[1], "r");
while (fscanf(file, "%d %d %d", &pas[PC], &pas[PC + 1], &pas[PC + 2]) != EOF) {
    PC += 3;
}
fclose(file);
while(EOP){
     // Fetch Cycle:
    IR[0] = pas[PC];      // OP
    IR[1] = pas[PC + 1];  // L
    IR[2] = pas[PC + 2];  // M
    PC += 3;              // Increment PC for next instruction
    switch (IR[0]) {
    case 1:
		// LIT
		//Literal push: sp <- sp- 1; pas[sp] <-n 
		SP-=1;    
		IR[SP] = IR[2];
		break;
    case 2: 
	// RTN
		if(IR[2] == 0)
		{ SP -=1;
		pas[SP] = pas[base(BP,IR[1]) - IR[2]];
		}
		else if (IR[2] ==1)
		// ADD
		{pas[SP+1]= (pas[SP+1]+pas[SP]);
		SP +=1;
		}
		else if (IR[2] ==2)
		// SUB
		{
		pas[SP+1] = (pas[SP+1] -pas[SP]);
		SP +=1;
		}
		else if (IR[2] ==3)
		//MUL
		{pas[SP+1]= (pas[SP+1]*pas[SP]);
		SP += 1;
		}
		else if (IR[2] ==4)
		// DIV
		{ pas[SP+1] = (pas[SP+1] == pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==5)
		// EQL
		{ pas[SP+1] = (pas[SP+1] == pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==6)
		// NEQ
		{ pas[SP+1] = (pas[SP+1] != pas[SP]);
		SP +=1;
		}
		else if (IR[2] ==7)
		// LSS
		{ pas[SP+1] = (pas[SP+1] < pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==8)
		//LEQ
		{ pas[SP+1] = (pas[SP+1] <= pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==9)
		// GTR
		{ pas[SP+1] = (pas[SP+1] > pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==10)
		// GEQ
		{pas[SP+1] = (pas[SP+1] >= pas[SP]);
			SP +=1;
		}
	// 02 RTN 0 0 Returns from a subroutine is encoded 0 0 0 and restores the caller’s AR:
	//sp ← bp + 1; bp ← pas[sp - 2];  pc ← pas[sp - 3];

	case 3: 
	// LOD
	//Load value to top of stack from the stack location at offset o from n lexicographical levels down
	//sp <- sp - 1;
	//pas[sp] <- pas[base(bp, n) - o];
	SP -=1;
	pas[SP] = pas[base(BP,IR[1]) - IR[2]];
	
	break;
	// STO
	case 4:
		pas[SP] = pas[base(BP, IR[1]) - IR[2]];
		SP = SP + 1;
		break;
	
	case 5:

	/*Call the procedure at code address a, generating a new activation record and setting PC to a:*/
	pas[SP - 1]  =  base(BP, IR[1]); /* static link (SL) */
	pas[SP - 2] = BP;	/* dynamic link (DL) */
	pas[SP - 3]  = PC;	 /*return address (RA)	*/ 
	BP = SP - 1;
	PC = IR[2];
	break;

	case 6:
		SP = SP - IR[2];
		break;
	case 7:
		PC = IR[2];
		break;
	case 8:
		if (pas[SP] == 0) {
			PC = IR[2];
		}
		else {
			SP = SP + 1;
		}
		break;
	case 9:
		if (IR[2] == 1) {
			printf("%d", pas[SP]);
			SP = SP+1;
		} else if (IR[2] == 2) {
			SP= SP-1;
			pas[SP] = getc(stdin);
		} else if (IR[2] == 3) {
			EOP = 0;
		}
		break;
	}
}
return 0;
}
