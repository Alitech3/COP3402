// Gabriel Flores Ali Chapman
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <string.h>
 
 #define MAX_IDENT_LENGTH 11
 #define MAX_NUMBER_LENGTH 5
 #define MAX_SYMBOL_TABLE_SIZE 500
 #define MAX_CODE_SIZE 500
 
 /* VM Op Codes */
 #define LIT 1   /* Load literal */
 #define OPR 2   /* Operation */
 #define LOD 3   /* Load variable */
 #define STO 4   /* Store variable */
 #define CAL 5   /* Call procedure */
 #define INC 6   /* Increment (allocate) */
 #define JMP 7   /* Jump */
 #define JPC 8   /* Jump conditional */
 #define SYS 9   /* System call (read/write/halt) */

typedef enum {
     modsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, fisym, eqlsym, neqsym, lessym, leqsym, gtrsym, 
     geqsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym,
     whilesym, dosym,  callsymNon, constsym, varsym, procsymNon, writesym, readsym, elsesym   
 } token_type;

 /* Token structure */
 typedef struct {
     int token;           // token type (numeric value)
     char lexeme[12];     // lexeme string (identifier, number, etc.)
 } Token;
 
 // IR
 typedef struct {
     int op;   // operation code (numerical)
     int l;    // lexicographical level
     int m;    // modifier (address, literal, etc.)
 } Instruction;
 
 /* Symbol Table Entry structure */
 typedef struct {
     int kind;         // 1 = const, 2 = var, 3 = proc
     char name[12];    // identifier name
     int val;          // constant value (if kind == 1)
     int level;        // lexical level (always 0 in this assignment)
     int addr;         // memory address (for variables)
     int mark;         // mark flag (0 = available, 1 = marked/unavailable)
 } symbol;
 
 char* reservedWords[] = {"const", "var", "procedure", "call", "begin", "end",
                          "if", "fi", "then", "else", "while", "do", "read", "write"};
 char* specialSymbols[] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":"};

 symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
 int symbolCount = 0;
 
 Instruction code[MAX_CODE_SIZE];
 int codeIndex = 0;
 
 FILE *source;
 Token currentToken;
 
 // wanted to be "fancy" for once lmfao
 void program(void);
 void block(void);
 void constDeclaration(void);
 int varDeclaration(void);
 void statement(void);
 void condition(void);
 void expression(void);
 void term(void);
 void factor(void);
 
 void error(const char *msg) {
    printf("Error: %s\n", msg);
    //  exit(1);
    return;
}
 
 /* Emit a new instruction to the code array */
 void emit(int op, int l, int m) {
     if (codeIndex >= MAX_CODE_SIZE) {
         error("Code array overflow");
     }
     code[codeIndex].op = op;
     code[codeIndex].l = l;
     code[codeIndex].m = m;
     codeIndex++;
 }
 
 int findSymbol(char *name) {
     for (int i = 0; i < symbolCount; i++) {
         if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].mark == 0)
             return i;
     }
     return -1;
 }
 
 /* Add a new symbol table entry */
 void addSymbol(int kind, char *name, int val, int level, int addr) {
     if (findSymbol(name) != -1) {
         error("symbol name has already been declared");
     }
     if (symbolCount >= MAX_SYMBOL_TABLE_SIZE) {
         error("Symbol table overflow");
     }
     symbolTable[symbolCount].kind = kind;
     strcpy(symbolTable[symbolCount].name, name);
     symbolTable[symbolCount].val = val;
     symbolTable[symbolCount].level = level;
     symbolTable[symbolCount].addr = addr;
     symbolTable[symbolCount].mark = 0;
     symbolCount++;
 }
 
 //Check valid special symbols
 int isSpecialSymbolChar(char c) {
     char str[2] = {c, '\0'};
     for (int i = 0; i < 13; i++) {
         if (strcmp(specialSymbols[i], str) == 0)
             return 1;
     }
     return 0;
 }
 
 Token getNextToken(FILE *input) {
     int lexemeIndex = 0;
     char currentLexeme[12];
     int c;
     Token token;
     token.token = modsym;  // default
 
     // skip whitespace
     while ((c = fgetc(input)) != EOF && isspace(c));
 
     if (c == EOF) {
         token.token = -1;
         return token;
     }
 
     // Handle comments
     if (c == '/') {
         int next = fgetc(input);
         if (next == '*') {
             long commentStart = ftell(input) - 2;
             int foundClosing = 0;
             int prev = 0;
             int ch;
             while ((ch = fgetc(input)) != EOF) {
                 if (prev == '*' && ch == '/') {
                     foundClosing = 1;
                     break;
                 }
                 prev = ch;
             }
             if (foundClosing) {
                 return getNextToken(input);
             } else {
                 fseek(input, commentStart, SEEK_SET);
                 lexemeIndex = 0;
                 currentLexeme[lexemeIndex++] = fgetc(input);
                 currentLexeme[lexemeIndex] = '\0';
                 token.token = slashsym;
                 strcpy(token.lexeme, currentLexeme);
                 return token;
             }
         } else {
             ungetc(next, input);
             lexemeIndex = 0;
             currentLexeme[lexemeIndex++] = c;
             currentLexeme[lexemeIndex] = '\0';
             token.token = slashsym;
             strcpy(token.lexeme, currentLexeme);
             return token;
         }
     }
 
     /* Identifiers and reserved words */
     if (isalpha(c)) {
         currentLexeme[lexemeIndex++] = c;
         while ((c = fgetc(input)) != EOF && (isalpha(c) || isdigit(c))) {
             if (lexemeIndex < MAX_IDENT_LENGTH)
                 currentLexeme[lexemeIndex++] = c;
             else {
                 error("Identifier too long");
             }
         }
         if (c != EOF)
             ungetc(c, input);
         currentLexeme[lexemeIndex] = '\0';
 
         int isReserved = 0;
         for (int i = 0; i < 14; i++) {
             if (strcmp(currentLexeme, reservedWords[i]) == 0) {
                 switch (i) {
                     case 0: token.token = constsym; break;
                     case 1: token.token = varsym; break;
                     case 2: token.token = procsymNon; break;
                     case 3: token.token = callsymNon; break;
                     case 4: token.token = beginsym; break;
                     case 5: token.token = endsym; break;
                     case 6: token.token = ifsym; break;
                     case 7: token.token = fisym; break;
                     case 8: token.token = thensym; break;
                     case 9: token.token = elsesym; break;
                     case 10: token.token = whilesym; break;
                     case 11: token.token = dosym; break;
                     case 12: token.token = readsym; break;
                     case 13: token.token = writesym; break;
                     default: token.token = identsym; break;
                 }
                 isReserved = 1;
                 break;
             }
         }
         if (!isReserved) {
             if (strcmp(currentLexeme, "mod") == 0)
                 token.token = modsym;
             else
                 token.token = identsym;
         }
         strcpy(token.lexeme, currentLexeme);
         return token;
     }
 
     /* Numbers */
     if (isdigit(c)) {
         currentLexeme[lexemeIndex++] = c;
         while ((c = fgetc(input)) != EOF && isdigit(c)) {
             if (lexemeIndex < MAX_NUMBER_LENGTH)
                 currentLexeme[lexemeIndex++] = c;
             else {
                 error("Number too long");
             }
         }
         if (c != EOF)
             ungetc(c, input);
         currentLexeme[lexemeIndex] = '\0';
         token.token = numbersym;
         strcpy(token.lexeme, currentLexeme);
         return token;
     }
 
     /* Special symbols */
     if (isSpecialSymbolChar(c)) {
         int next;
         currentLexeme[lexemeIndex++] = c;
         currentLexeme[lexemeIndex] = '\0';
         switch (c) {
             case '+': token.token = plussym; break;
             case '-': token.token = minussym; break;
             case '*': token.token = multsym; break;
             case '/': token.token = slashsym; break;
             case '(': token.token = lparentsym; break;
             case ')': token.token = rparentsym; break;
             case '=': token.token = eqlsym; break;
             case ',': token.token = commasym; break;
             case ';': token.token = semicolonsym; break;
             case '.': token.token = periodsym; break;
             case '<': token.token = lessym; break;
             case '>': token.token = gtrsym; break;
             case ':':
                 next = fgetc(input);
                 if (next == '=') {
                     currentLexeme[lexemeIndex++] = '=';
                     currentLexeme[lexemeIndex] = '\0';
                     token.token = becomessym;
                     strcpy(token.lexeme, currentLexeme);
                     return token;
                 } else {
                     ungetc(next, input);
                     token.token = becomessym;
                     strcpy(token.lexeme, currentLexeme);
                     return token;
                 }
                 break;
             default: break;
         }
         strcpy(token.lexeme, currentLexeme);
         return token;
     }
 
     // unexpected characters
     {
         char errMsg[50];
         sprintf(errMsg, "Invalid character encountered '%c'", c);
         error(errMsg);
     }
     token.token = -1;
     return token;
 }
 
 void program(void) {
     block();
     if (currentToken.token != periodsym && currentToken.token != endsym) {
        error("program must end with period");
     }
     emit(SYS, 0, 3);
 }
 
 void block(void) {
     constDeclaration();
     int numVars = varDeclaration();
    
     emit(INC, 0, 3 + numVars);
     statement();
 }
 
 void constDeclaration(void) {
     if (currentToken.token == constsym) {
         currentToken = getNextToken(source);
         if (currentToken.token != identsym)
             error("const, var, and read keywords must be followed by identifier");
         while (1) {
             char name[12];
             strcpy(name, currentToken.lexeme);
             currentToken = getNextToken(source);
             if (currentToken.token != eqlsym)
                 error("constants must be assigned with =");
             currentToken = getNextToken(source);
             if (currentToken.token != numbersym)
                 error("constants must be assigned an integer value");
             int value = atoi(currentToken.lexeme);
             addSymbol(1, name, value, 0, 0);
             currentToken = getNextToken(source);
             if (currentToken.token == commasym) {
                 currentToken = getNextToken(source);
                 if (currentToken.token != identsym)
                     error("const, var, and read keywords must be followed by identifier");
                 continue;
             } else if (currentToken.token == semicolonsym) {
                 currentToken = getNextToken(source);
                 break;
             } else {
                 error("constant declarations must be followed by a semicolon");
             }
         }
     }
 };


 int varDeclaration(void) {
     int numVars = 0;
     if (currentToken.token == varsym) {
         currentToken = getNextToken(source);
         if (currentToken.token != identsym)
             error("const, var, and read keywords must be followed by identifier");
         while (1) {
             char name[12];
             strcpy(name, currentToken.lexeme);
             addSymbol(2, name, 0, 0, 3 + numVars);
             numVars++;
             currentToken = getNextToken(source);
             if (currentToken.token == commasym) {
                 currentToken = getNextToken(source);
                 if (currentToken.token != identsym)
                     error("const, var, and read keywords must be followed by identifier");
             } else if (currentToken.token == semicolonsym) {
                 currentToken = getNextToken(source);
                 break;
             } else {
                 error("variable declarations must be followed by a semicolon");
             }
         }
     }
     return numVars;
 }

 void statement(void) {
     if (currentToken.token == identsym) {
         int symIndex = findSymbol(currentToken.lexeme);
         if (symIndex == -1) {
             char errMsg[50];
             sprintf(errMsg, "undeclared identifier %s", currentToken.lexeme);
             error(errMsg);
         }
         if (symbolTable[symIndex].kind != 2)
             error("only variable values may be altered");
         currentToken = getNextToken(source);
         if (currentToken.token != becomessym)
             error("assignment statements must use :=");
         currentToken = getNextToken(source);
         expression();
         emit(STO, 0, symbolTable[symIndex].addr);
     }
     else if (currentToken.token == beginsym) {
         currentToken = getNextToken(source);
         statement();
         while (currentToken.token == semicolonsym) {
             currentToken = getNextToken(source);
             statement();
         }
         if (currentToken.token != endsym)
             error("begin must be followed by end");
         currentToken = getNextToken(source);
     }
     else if (currentToken.token == ifsym) {
         currentToken = getNextToken(source);
         condition();
         int jpcIndex = codeIndex;
         emit(JPC, 0, 0);
         if (currentToken.token != thensym)
             error("if must be followed by then");
         currentToken = getNextToken(source);
         statement();
         code[jpcIndex].m = codeIndex;
     }
     else if (currentToken.token == whilesym) {
         int loopIndex = codeIndex;
         currentToken = getNextToken(source);
         condition();
         int jpcIndex = codeIndex;
         emit(JPC, 0, 0);
         if (currentToken.token != dosym)
             error("while must be followed by do");
         currentToken = getNextToken(source);
         statement();
         emit(JMP, 0, loopIndex);
         code[jpcIndex].m = codeIndex;
     }
     else if (currentToken.token == readsym) {
         currentToken = getNextToken(source);
         if (currentToken.token != identsym)
             error("const, var, and read keywords must be followed by identifier");
         int symIndex = findSymbol(currentToken.lexeme);
         if (symIndex == -1) {
             char errMsg[50];
             sprintf(errMsg, "undeclared identifier %s", currentToken.lexeme);
             error(errMsg);
         }
         if (symbolTable[symIndex].kind != 2)
             error("only variable values may be altered");
         currentToken = getNextToken(source);
         emit(SYS, 0, 2);
         emit(STO, 0, symbolTable[symIndex].addr);
     }
     else if (currentToken.token == writesym) {
         currentToken = getNextToken(source);
         expression();
         emit(SYS, 0, 1);
     }
     
 }
 
 void condition(void) {
     expression();
     int relOp = currentToken.token;
     if (relOp != eqlsym && relOp != neqsym && relOp != lessym &&
         relOp != leqsym && relOp != gtrsym && relOp != geqsym)
         error("condition must contain comparison operator");
     currentToken = getNextToken(source);
     expression();
     switch (relOp) {
         case eqlsym:  emit(OPR, 0, 8);  break;
         case neqsym:  emit(OPR, 0, 9);  break;
         case lessym:  emit(OPR, 0, 10); break;
         case leqsym:  emit(OPR, 0, 11); break;
         case gtrsym:  emit(OPR, 0, 12); break;
         case geqsym:  emit(OPR, 0, 13); break;
         default: break;
     }
 }

 void expression(void) {
     int isNegative = 0;
     if (currentToken.token == minussym) {
         isNegative = 1;
         currentToken = getNextToken(source);
     }
     term();
     if (isNegative)
         emit(OPR, 0, 1);  
     while (currentToken.token == plussym || currentToken.token == minussym) {
         int op = currentToken.token;
         currentToken = getNextToken(source);
         term();
         if (op == plussym)
             emit(OPR, 0, 2); 
         else
             emit(OPR, 0, 3);  
     }
 }

 void term(void) {
     factor();
     while (currentToken.token == multsym ||
            currentToken.token == slashsym ||
            currentToken.token == modsym) {
         int op = currentToken.token;
         currentToken = getNextToken(source);
         factor();
         if (op == multsym)
             emit(OPR, 0, 4);  
         else if (op == slashsym)
             emit(OPR, 0, 5);  
         else if (op == modsym)
             emit(OPR, 0, 11); 
     }
 }

 void factor(void) {
     if (currentToken.token == identsym) {
         int symIndex = findSymbol(currentToken.lexeme);
         if (symIndex == -1) {
             char errMsg[50];
             sprintf(errMsg, "undeclared identifier %s", currentToken.lexeme);
             error(errMsg);
         }
         if (symbolTable[symIndex].kind == 1)
             emit(LIT, 0, symbolTable[symIndex].val);
         else if (symbolTable[symIndex].kind == 2)
             emit(LOD, 0, symbolTable[symIndex].addr);
         else {
             error("invalid identifier in expression");
         }
         currentToken = getNextToken(source);
     }
     else if (currentToken.token == numbersym) {
         emit(LIT, 0, atoi(currentToken.lexeme));
         currentToken = getNextToken(source);
     }
     else if (currentToken.token == lparentsym) {
         currentToken = getNextToken(source);
         expression();
         if (currentToken.token != rparentsym)
             error("right parenthesis must follow left parenthesis");
         currentToken = getNextToken(source);
     }
     else {
         error("arithmetic equations must contain operands, parentheses, numbers, or symbols");
     }
 }

 // print assembly
 void printCode(void) {
     printf("Assembly Code:\n");
     printf("Line\tOP\tL\tM\n");
     for (int i = 0; i < codeIndex; i++) {
         char opName[10];
         switch (code[i].op) {
             case LIT: strcpy(opName, "LIT"); break;
             case OPR:
                 if (code[i].m == 0)       strcpy(opName, "RET");
                 else if (code[i].m == 1)  strcpy(opName, "NEG");
                 else if (code[i].m == 2)  strcpy(opName, "ADD");
                 else if (code[i].m == 3)  strcpy(opName, "SUB");
                 else if (code[i].m == 4)  strcpy(opName, "MUL");
                 else if (code[i].m == 5)  strcpy(opName, "DIV");
                 else if (code[i].m == 6)  strcpy(opName, "ODD");
                 else if (code[i].m == 8)  strcpy(opName, "EQL");
                 else if (code[i].m == 9)  strcpy(opName, "NEQ");
                 else if (code[i].m == 10) strcpy(opName, "LSS");
                 else if (code[i].m == 11) strcpy(opName, "MOD");
                 else if (code[i].m == 12) strcpy(opName, "GTR");
                 else if (code[i].m == 13) strcpy(opName, "GEQ");
                 else                      strcpy(opName, "OPR");
                 break;
             case LOD:  strcpy(opName, "LOD"); break;
             case STO:  strcpy(opName, "STO"); break;
             case CAL:  strcpy(opName, "CAL"); break;
             case INC:  strcpy(opName, "INC"); break;
             case JMP:  strcpy(opName, "JMP"); break;
             case JPC:  strcpy(opName, "JPC"); break;
             case SYS:  strcpy(opName, "SYS"); break;
             default:   strcpy(opName, "???"); break;
         }
         printf("%d\t%s\t%d\t%d\n", i, opName, code[i].l, code[i].m);
     }
 }
 
// symbol table
 void printSymbolTable(void) {
     printf("Symbol Table:\n");
     printf("Kind\tName\tValue\tLevel\tAddress\tMark\n");
     printf("---------------------------------------------------\n");
     for (int i = 0; i < symbolCount; i++) {
         printf("%d\t%s\t%d\t%d\t%d\t%d\n",
                symbolTable[i].kind,
                symbolTable[i].name,
                symbolTable[i].val,
                symbolTable[i].level,
                symbolTable[i].addr,
                symbolTable[i].mark);
     }
 }
 
 void markVariables(void) {
     for (int i = 0; i < symbolCount; i++) {
         if (symbolTable[i].kind == 2)
             symbolTable[i].mark = 1;
     }
 }

 int main(int argc, char *argv[]) {
    char outputFilename[100];
     if (argc < 1) {
         printf("Error: No input file provided\n");
         return 1;
     }
     source = fopen(argv[1], "r");
     if (source == NULL) {
         printf("Error opening file.\n");
         return 1;
     }
 
     codeIndex = 0;
     symbolCount = 0;
 
     emit(JMP, 0, 13);

     currentToken = getNextToken(source);
     program();
     markVariables();
 
     printCode();
     printSymbolTable();

    FILE *output = fopen("elf.txt", "w");
    if (output == NULL) {
        printf("Error opening elf.txt for writing\n");
        return 1;
    }
    for (int i = 0; i < codeIndex; i++) {
        fprintf(output, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
    }
    fclose(output);
    fclose(source);

    return 0;
};
