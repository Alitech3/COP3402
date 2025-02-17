#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_IDENT_LENGTH 11
#define MAX_NUMBER_LENGTH 5

// token types
typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, fisym,
    eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym,
    commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym,
    thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym
} token_type;

// reserved words
char* reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "fi", "then", "else", "while", "do", "read", "write"};
// special symbols
char* specialSymbols[] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":"};

// token representation
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

Token getNextToken(FILE *input) {
    int lexemeIndex = 0;
    char currentLexeme[12];
    int c;  
    Token token;
    token.token = skipsym;

    // Skip whitespace
    while ((c = fgetc(input)) != EOF && (c == ' ' || c == '\t' || c == '\n'))
        ;

    if (c == EOF) {
        token.token = -1;
        return token;
    }

    if (c == '/') {
        int next = fgetc(input);
        if (next == '*') {
            // We may have a multi-line comment.
            // Save the file position before processing the comment.
            long commentStart = ftell(input) - 2;  // position before reading "/" and "*"
            int foundClosing = 0;
            int prev = 0;
            int ch;
            // Attempt to skip over the comment.
            while ((ch = fgetc(input)) != EOF) {
                if (prev == '*' && ch == '/') {
                    foundClosing = 1;
                    break;
                }
                prev = ch;
            }
            if (foundClosing) {
                // Comment properly closed. Ignore comment and get the next token.
                return getNextToken(input);
            } else {
                // No closing "*/" was found.
                // Backtrack to the beginning of the potential comment.
                fseek(input, commentStart, SEEK_SET);
                // Now process the '/' normally (as a division operator).
                lexemeIndex = 0;
                currentLexeme[lexemeIndex++] = fgetc(input);  // should be '/'
                currentLexeme[lexemeIndex] = '\0';
                token.token = slashsym;
                strcpy(token.lexeme, currentLexeme);
                return token;
            }
        } else {
            // Not a comment un-read the character after /
            ungetc(next, input);
            lexemeIndex = 0;
            currentLexeme[lexemeIndex++] = c;
            currentLexeme[lexemeIndex] = '\0';
            token.token = slashsym;
            strcpy(token.lexeme, currentLexeme);
            return token;
        }
    }
    
    // If the token is an identifier (starts with a letter)
    if (isalpha(c)) {
        currentLexeme[lexemeIndex++] = c;
        // Continue reading letters and digits
        while ((c = fgetc(input)) != EOF && (isalpha(c) || isdigit(c))) {
            if (lexemeIndex < MAX_IDENT_LENGTH)
                currentLexeme[lexemeIndex++] = c;
            else {
                printf("Error: Identifier too long\n");
                token.token = -1;
                return token;
            }
        }
        // Put back the non-valid character (if not EOF)
        if (c != EOF)
            ungetc(c, input);
        currentLexeme[lexemeIndex] = '\0';

        // Check if the identifier is a reserved word:
        int isReserved = 0;
        for (int i = 0; i < 14; i++) {
            if (strcmp(currentLexeme, reservedWords[i]) == 0) {
                // Map reserved word to its token type explicitly:
                switch(i) {
                    case 0: token.token = constsym; break;
                    case 1: token.token = varsym; break;
                    case 2: token.token = procsym; break;
                    case 3: token.token = callsym; break;
                    case 4: token.token = beginsym; break;
                    case 5: token.token = endsym; break;
                    case 6: token.token = ifsym; break;
                    default: break;
                }
                isReserved = 1;
                break;
            }
        }
        if (!isReserved)
            token.token = identsym;
        strcpy(token.lexeme, currentLexeme);
        return token;
    }

    // If the token is a number
    if (isdigit(c)) {
        currentLexeme[lexemeIndex++] = c;  // store the first digit
        while ((c = fgetc(input)) != EOF && isdigit(c)) {
            if (lexemeIndex < MAX_NUMBER_LENGTH)
                currentLexeme[lexemeIndex++] = c;
            else {
                printf("Error: Number too long\n");
                token.token = -1;
                return token;
            }
        }
        if (c != EOF)
            ungetc(c, input);
        currentLexeme[lexemeIndex] = '\0';
        token.token = numbersym;
        strcpy(token.lexeme, currentLexeme);
        return token;
    }

    // If the token is a special symbol
    if (isSpecialSymbol(c)) {
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
                    // Found the assignment operator ":="
                    currentLexeme[lexemeIndex++] = c;  // add ':'
                    currentLexeme[lexemeIndex++] = '='; // add '='
                    currentLexeme[lexemeIndex] = '\0';
                    token.token = 20;  // Set token type to 20 as requested
                    strcpy(token.lexeme, currentLexeme);
                    return token;
                } else {
                    // Not a valid assignment operator; unget the character
                    ungetc(next, input);
                    // Optionally, handle ':' as its own token or as an error
                    currentLexeme[lexemeIndex++] = c;
                    currentLexeme[lexemeIndex] = '\0';
                    token.token = becomessym; // or another token code for a lone ':'
                    strcpy(token.lexeme, currentLexeme);
                    return token;
                }
                token.token = becomessym; 
                break;
            default: break;
        }
        strcpy(token.lexeme, currentLexeme);
        return token;
    }

    // Handle unexpected characters
    printf("Error: Invalid character encountered '%c'\n", c);
    token.token = -1;
    return token;
}

int main() {
    FILE *source = fopen("input.txt", "r"); 
    FILE *out = fopen("output.txt", "w");
    if (source == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    if (out == NULL) {
        printf("Error opening the file.\n");
        return 1;  // Return an error code if file couldn't be opened
    }

    printf("Source Program:\n");
    fprintf(out, "Source Program:\n");
    char ch;
    while ((ch = fgetc(source)) != EOF) {
        putchar(ch);
    }
    rewind(source);
    printf("\nLexemes Table:\n");
    fprintf(out, "Lexemes Table:\n");

    printf("%5s %5s %5s\n", "lexeme", "token", "type");
    fprintf(out, "%5s %5s %5s\n", "lexeme", "token", "type");
    while (1) {
        Token token = getNextToken(source);
        if (token.token == -1)
            break;
        printf("%5s %5s %5d\n", token.lexeme, " ", token.token);
        fprintf(out,"%5s %5s %5d\n", token.lexeme, " ", token.token);
    }
    rewind(source);
    while (1) {
        Token token = getNextToken(source);
        if (token.token == -1) {
            break;
        }
        // If the token is an identifier or number, print the token code,
        // the length of the lexeme, and the lexeme itself.
        if (token.token == identsym || token.token == numbersym) {
            printf("%d %s ", token.token, token.lexeme);
            fprintf(out,"%d %s ", token.token, token.lexeme);
        } else {
            // For other tokens, simply print the token code.
            printf("%d ", token.token);
            fprintf(out,"%d ", token.token);
        }
    }
    fclose(source);
    fclose(out);
    return 0;
}
