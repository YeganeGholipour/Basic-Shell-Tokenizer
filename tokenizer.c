#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXTOKENS 1000 // what we return as tokens 
#define MAXLEN 100
#define SPECIALLEN 5

char special_characters[SPECIALLEN] = {'>', '<', '&', '|', '$'};

int tokenize(char argument[], int max_len);
void printTokens(char *tokens[], int token_num);

// MEMORY RELATED FUNCTIONS
int allocateMemory(char argument[], char *tokens[], int max_tokens, int *token_num);
void freeMemory(char *tokens[], int token_num);

// HELPER FUNCTIONS
bool isspecial(char character);
bool is_valid_double_operator(char first, char second);

// FUNCTIONS FOR HANDLING DIFFERENT INPUTS
int handle_double_quotes(char *argument, int max_len);
int handle_single_quotes(char *argument, int max_len);
int handle_special_characters(char *argument, char character, int max_len);

int main(void) {
    char *tokens[MAXTOKENS];
    char arg[MAXLEN];
    int arg_status, token_status, token_num = 0;

    while (1) {
        arg_status = tokenize(arg, MAXLEN);

        // Check for real EOF (Ctrl+D)
        if (arg_status == EOF || feof(stdin)) {
            fprintf(stderr, "Detected EOF (Ctrl+D), exiting...\n");
            if (token_num > 0) {
                printTokens(tokens, token_num);
                freeMemory(tokens, token_num);
            }
            break;
        }

        if (arg_status < 0) {
            // Handle unmatched quote
            fprintf(stderr, "Unmatched quote, exiting.\n");
            break;
        }

        token_status = allocateMemory(arg, tokens, MAXTOKENS, &token_num);
        if (token_status < 0) {
            fprintf(stderr, "Error While Allocating Memory!\n");
            return 1;
        }
    }

    return 0;
}


void freeMemory(char *tokens[], int token_num) {
    for (int i = 0; i < token_num; i++) 
        free(tokens[i]);
}

void printTokens(char *tokens[], int token_num) {
    printf("****************************\n");
    printf("CALLING FUNCTION: printTokens\n");
    for (int i = 0; i < token_num; i++)
        printf("%s\n", tokens[i]);
}

int allocateMemory(char argument[], char *tokens[], int max_tokens, int *token_num) {
    char *p;
    if (*token_num < max_tokens && ((p = (char *)malloc(strlen(argument)+1)) != NULL)) {
        printf("****************************\n");
        printf("CALLING FUNCTION: allocateMemory\n");
        printf("ARGUMENT: %s\n", argument);
        strcpy(p, argument);
        tokens[(*token_num)++] = p;
    } else
        return -1;

    return 0;
}   

int tokenize(char argument[], int max_len) {
    int c;

    // skip the whitespaces
    while (isspace(c = getchar()))
        ;
    
    if (c == EOF)
        return EOF;

    /* Process the first non space character */
    /* handle double quotes and escape characters */
    if (c == '"') {
        printf("********************************\n");
        printf("HANDLING DOUBLE QUOTES\n");
        return handle_double_quotes(argument, max_len); 
    }

    /* handle single quotes*/
    if (c == '\'') {
        printf("********************************\n");
        printf("HANDLING SINGLE QUOTES\n");
        return handle_single_quotes(argument, max_len);
    }

    /* Process Special Characters */
    if (isspecial(c)) {
        printf("********************************\n");
        printf("HANDLING SPECIAL CHARACTERS\n");
        return handle_special_characters(argument, c, max_len);
    }

    /* Process normal characters: A-Z, a-Z, 1-9, / */   
    else {
        printf("********************************\n");
        printf("HANDLING NON SPECIAL CHARACTERS\n");
        while (max_len > 1 && !isspecial(c) && !isspace(c)) {
            *argument++ = c;
            c = getchar();
        }
        ungetc(c, stdin); //put c back to buffer to process it the next iteration
    }

    *argument = '\0';
    return 0; 
}

bool isspecial(char character) {
    for (int i = 0; i < SPECIALLEN; i++)
        if (character == special_characters[i])
            return true;

    return false;
}

int handle_special_characters(char *argument, char character, int max_len) {
    int c;
    if (max_len > 1) {
        *argument++ = character;
        max_len--;

        c = getchar();
        if (max_len > 1 && is_valid_double_operator(character, c)) {
            *argument++ = c;
            max_len--;
        } else if (c == EOF) 
            return EOF;
        else
            ungetc(c, stdin);

        *argument = '\0';
        return 0;
    } else
        return -1;
}

bool is_valid_double_operator(char first, char second) {
    return (first == '>' && second == '>') ||
           (first == '<' && second == '<') ||
           (first == '&' && second == '&') ||
           (first == '|' && second == '|');
}

int handle_single_quotes(char *argument, int max_len) {
    int c;

    while (max_len > 1 && (c = getchar()) != EOF) {
        if (c == '\'')
            break;
        else {
            *argument++ = c;
            max_len--;
        }
    }

    if (c == EOF) {
        fprintf(stderr, "Error: unmatched single quote\n");
        return -1; // -1 means there was unmatched quote
    }

    *argument = '\0';
    return 0;
}

int handle_double_quotes(char *argument, int max_len) {
    int c, next;

    while (max_len > 1 && (c = getchar()) != EOF) {
        if (c == '\\') {
            next = getchar();
            if (next == EOF) {
                fprintf(stderr, "Error: unmatched double quote\n");
                return -1;
            }
            *argument++ = next;
            max_len--;
        } 
        else if (c == '"')
            break;
        else {
            printf("Getting normal characters...");
            *argument++ = c;
            max_len--;
        }
    }

    
    if (c == EOF) {
        fprintf(stderr, "Error: unmatched double quote\n");
        return -1; // -1 means there was unmatched quote
    }

    *argument = '\0'; 
    return 0;
}