/*============================================================================
| Author: Gabriel Flores & Ali Chapman
| ===========================================================================*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 500
#define TEXT_START 10

// CPU Registers
int BP = 499, SP = 500, PC = TEXT_START;
int IR[3] = {0};
int EOP = 1;

int pas[ARRAY_SIZE] = {0}; // Process Address Space (PAS) initialized to 0

void printAR(int PC, int BP, int SP, int OP, int M) {
    switch (OP) {
        case 1:
            printf("\n\tLIT ");
            break;
        case 2:
            switch (M) {
                case 0: printf("\n\tRTN "); break;
                case 1: printf("\n\tADD "); break;
                case 2: printf("\n\tSUB "); break;
                case 3: printf("\n\tMUL "); break;
                case 4: printf("\n\tDIV "); break;
                case 5: printf("\n\tEQL "); break;
                case 6: printf("\n\tNEQ "); break;
                case 7: printf("\n\tLSS "); break;
                case 8: printf("\n\tLEQ "); break;
                case 9: printf("\n\tGTR "); break;
                case 10: printf("\n\tGEQ "); break;
                default: break;
            }
            break;
        case 3: printf("\n\tLOD "); break;
        case 4: printf("\n\tSTO "); break;
        case 5: printf("\n\tCAL "); break;
        case 6: printf("\n\tINC "); break;
        case 7: printf("\n\tJMP "); break;
        case 8: printf("\n\tJPC "); break;
        case 9: printf("\n\tSYS "); break;
        default: break;
    }

    // Print instruction arguments
    printf("%d %-7d", IR[1], IR[2]);

    // Print stack information
    printf("%d %5d %5d ", PC, BP, SP);
    if (SP < ARRAY_SIZE) {
        for (int i = ARRAY_SIZE - 1; i >= SP; i--) {
            if (pas[i] != pas[i+1]){
                if((pas[i-1] == ARRAY_SIZE-1)&&(pas[i]==ARRAY_SIZE-1)) printf("%2s", " | ");
                else if(pas[i] == ARRAY_SIZE-1) printf("%2s", " | ");
            }

            printf("%d ", pas[i]);
		    // If we encounter the dynamic link (stored at pas[currentSP]), we print the separator
        }
    }
}

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

printf("               PC   BP   SP    stack\n");
printf("Inital Values: %d   %d  %d\n", PC, BP, SP);

FILE *file = fopen(argv[1], "r");
while (fscanf(file, "%d %d %d", &pas[PC], &pas[PC + 1], &pas[PC + 2]) != EOF) {
    // printf("%d %d %d\n", pas[PC], pas[PC + 1], pas[PC + 2]);
    PC += 3;
}
fclose(file);
PC = 10;
while(EOP) {
    // Fetch Cycle:	
    IR[0] = pas[PC];      // OP
    IR[1] = pas[PC + 1];  // L
    IR[2] = pas[PC + 2];  // M
    PC += 3;              // Increment PC for next instruction
    switch (IR[0]) {
	case 1:
    // LIT: Push literal onto the stack
    SP -= 1;
    pas[SP] = IR[2];
    break;
    case 2: 
	// RTN
	    if (IR[2] == 0) {
        SP = BP + 1;                  // Reset stack pointer
        BP = pas[SP - 2];             // Restore base pointer
        PC = pas[SP - 3];             // Set PC to the return address
		}
		else if (IR[2] == 1)
		// ADD
		{
            pas[SP+1] = pas[SP+1]+pas[SP];
		SP +=1;
		}
		else if (IR[2] ==2)
		// SUB
		{
		pas[SP+1] = pas[SP+1]-pas[SP];
		SP +=1;
		}
		else if (IR[2] ==3)
		//MUL
		{
            pas[SP+1]= (pas[SP+1]*pas[SP]);
		SP += 1;
		}
		else if (IR[2] ==4)
		// DIV
		{ 
            pas[SP+1] = (pas[SP+1]/ pas[SP]);
			SP +=1;
		}
		else if (IR[2] ==5)
		// EQL
		{ 
            pas[SP+1] = (pas[SP+1] == pas[SP]);
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
        break;
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
		pas[base(BP, IR[1]) - IR[2]] = pas[SP];
		SP += 1;
		break;
	case 5: 
    // CAL: Call the procedure at code address a, generating a new activation record and setting PC to a
    pas[SP - 1]  = base(BP, IR[1]);  // static link (SL)
    pas[SP - 2] = BP;                // dynamic link (DL)
    pas[SP - 3]  = PC;               // return address (RA)
    BP = SP - 1;                     // update BP for the new AR
    PC = IR[2];                      // jump to the procedure
    break;
	case 6:
        // INC
		// Allocate n locals on the stack
        SP = SP - IR[2];
		break;
	case 7:
        // JMP
		PC = IR[2];
		break;
	case 8:
    // JPC: Jump if the top of the stack is zero
    if (pas[SP] == 0) {
        PC = IR[2];  // Jump to the specified address
    } else {
        SP++;        // Otherwise, pop the value from the stack
    }
    break;

	case 9:
        // SIO
		if (IR[2] == 1) {
            printf("\nOutput result is: %d", pas[SP]);
			SP = SP+1;
		} else if (IR[2] == 2) {
			SP -= 1;
			printf("\nPlease enter an integer: ");
			fscanf(stdin, "%d%*c", &pas[SP]);
		} else if (IR[2] == 3) {
			EOP = 0;
		}
		break;
	}
    printAR(PC, BP, SP, IR[0], IR[2]);
}
return 0;
}
