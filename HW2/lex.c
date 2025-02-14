#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_IDENT_LENGTH 11
#define MAX_NUMBER_LENGTH 5

// Token types
typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
    eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym,
    commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym,
    thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym, error
} token_type;

//finish the comment program latter
//int comment(char*str){
  //  char comment1 = "/*";
    //char comment2 = "*/";
    // if(strcmp(str,comment1) == 0)
//}

char* reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"};
// Define special symbols
char* specialSymbols[] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":"};

// Define token representation
typedef struct {
    int token;  // The token number
    char lexeme[12];  // The lexeme (identifier, number, etc.)
} Token;

// Function to determine if a character is a letter
int isLetter(char c) {
    return (isalpha(c));
}

// Function to determine if a character is a digit
int isDigit(char c) {
    return (isdigit(c));
}

// Function to check if a string is a reserved word
int isReservedWord(char* str) {
    for (int i = 0; i < 14; i++) {
        if (strcmp(str, reservedWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a character is a valid special symbol
int isSpecialSymbol(char c) {
    char specialChar[] = {c, '\0'};
    for (int i = 0; i < 13; i++) {
        if (strcmp(specialSymbols[i], specialChar) == 0) {
            return 1;
        }
    }
    return 0;
}

Token getNextToken(FILE* input){
    int lexemeIndex = 0;
    char currentLexeme[12];
    char currentchar;
    // this makes the current token SKIPSYM and we can change it later
    Token token;
    token.token = skipsym;
    currentchar = fgetc(input);

    printf("Current char: %d\n", currentchar);

    if(currentchar == EOF){
        printf("ehybaby");
        token.token = -1;
        return token;
    }
    // This if statement detects if its a identifier or a reserved word
    if(isLetter(currentchar)){
        if(lexemeIndex < MAX_IDENT_LENGTH){
            currentLexeme[lexemeIndex++] = currentchar;
        }
        else {
        printf("Error: Identifier too long long\n");
        token.token = -1;
        return token;
        }
        // Check if its a reserved word 
    if(isReservedWord(currentLexeme))
        for(int i = 0; i < 14;i++){
        if(strcmp(currentLexeme,reservedWords[i]) == 0){
            token.token = constsym + i;
            break;
        }
    }
    else{
        token.token = identsym;
    }
    currentLexeme[lexemeIndex] = '\0';
    strcpy(token.lexeme, currentLexeme);
    return token;
}
    // check if its a number
    if(isDigit(currentchar)){
        while((currentchar = fgetc(input))!= EOF && isDigit(currentchar)){
        if(lexemeIndex < MAX_NUMBER_LENGTH) {
        currentLexeme[lexemeIndex++] = currentchar;
        }
        else{
            printf("Error: Number too long\n");
            token.token = -1;
            return token;
        }
    }
    currentLexeme[lexemeIndex] = "\0";
    token.token = numbersym;
    strcpy(token.lexeme,currentLexeme);
    return token;
}
    // Handle special symbols (operators and punctuation)
        if (isSpecialSymbol(currentchar)) {
            currentLexeme[lexemeIndex++] = currentchar;
            currentLexeme[lexemeIndex] = '\0';
            token.token = skipsym;  // Initialize as SKIPSYM (default)
            
            // Match special symbol
            switch (currentchar) {
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
                case ':': token.token = becomessym; break;
            }
            strcpy(token.lexeme, currentLexeme);
            return token;
        }
    
        // Handle invalid characters (error)
        printf("Error: Invalid character encountered '%c'\n", currentchar);
        token.token = -1;  // Error
        return token;
}

void main(void) {
    FILE *source = fopen("input.txt", "r");  // Replace with your file path
    if (source == NULL) {
        printf("Error opening file.\n");
        return;
    }
    printf("%s\n", "Source Program:");
    // char ch;
    // while ((ch = fgetc(source)) != EOF) {
    //     putchar(ch);
    // }
    printf("\n");
    printf("Lexemes Table:\n");
    printf("\n");

    while (1) {
        Token token = getNextToken(source);
        if (token.token == -1) {
            break;
        }
        // printf("Token: %d, Lexeme: %s\n", token.token, token.lexeme);
    }
    fclose(source);
    return;
}
