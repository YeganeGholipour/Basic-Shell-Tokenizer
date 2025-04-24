#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXTOKENS 1000
#define MAXLEN 100
#define SPECIALLEN 5

char special_characters[SPECIALLEN] = {'>', '<', '&', '|', '$'};

int tokenize(char *line, char *tokens[], int max_tokens, int max_len, int *token_num);
void printTokens(char *tokens[], int token_num);

// MEMORY FUNCTIONS
int *allocateMemory(char argument[], char *tokens[], int max_tokens, int *token_num);
void freeMemory(char *tokens[], int token_num);

// HELPER FUNCTIONS
bool isspecial(char character);
bool is_valid_double_operator(char first, char second);

// INPUT HANDLERS
char *handle_double_quotes(char argument[], char *p, int max_len);
char *handle_single_quotes(char argument[], char *p, int max_len);
char *handle_special_characters(char argument[], char *p, int max_len);

int main(void) {
    char *tokens[MAXTOKENS];
    char *buffer = NULL;
    size_t buffsize = 0;
    size_t read;
    int token_status, token_num;

    while (1) {
        printf("****************************\n");
        printf("tokenizer> ");
        read = getline(&buffer, &buffsize, stdin);

        if (read == -1) {
            fprintf(stderr, "Detected EOF (Ctrl+D), exiting...\n");
            break;
        }

        if (buffer[read - 1] == '\n')
            buffer[read - 1] = '\0';

        token_num = 0;
        token_status = tokenize(buffer, tokens, MAXTOKENS, MAXLEN, &token_num);

        if (token_status < 0) {
            fprintf(stderr, "Error: tokenizing input\n");
            continue;
        }

        printTokens(tokens, token_num);
        freeMemory(tokens, token_num);
    }

    free(buffer);
    return 0;
}

void freeMemory(char *tokens[], int token_num) {
    for (int i = 0; i < token_num; i++) 
        free(tokens[i]);
}

void printTokens(char *tokens[], int token_num) {
    printf("****************************\n");
    printf("CALLING FUNCTION: printTokens\n");
    for (int i = 0; i < token_num; i++) {
        printf("%s\n", tokens[i]);
    }
}

int *allocateMemory(char *argument, char *tokens[], int max_tokens, int *token_num) {
    char *p;
    if (*token_num < max_tokens && ((p = malloc(strlen(argument) + 1)) != NULL)) {
        // Debug
        // printf("****************************\n");
        // printf("CALLING FUNCTION: allocateMemory\n");
        // printf("ARGUMENT: %s\n", argument);
        strcpy(p, argument);
        tokens[(*token_num)++] = p;
    } else {
        return NULL;
    }

    return token_num;
}

int tokenize(char *line, char *tokens[], int max_tokens, int max_len, int *token_num) {
    char *p = line;
    char buffer[MAXLEN] = {0};
    int memory_status;

    *token_num = 0;

    printf("Line1: %s\n", line);

    while (*p != '\0') {
        while (isspace(*p)) p++;
        if (*p == '\0') break;

        printf("Buffer Befor Handling a token: %s\n", buffer);

        printf("****************************\n");
        printf("Processing character: %c\n", *p);
        printf("Line: %s\n", line);

        char *b = buffer;
        int len = max_len;

        if (*p == '"') {
            printf("Entering double quotes handler...\n");
            p = handle_double_quotes(buffer, p, len);
            printf("Buffer After Handling a token: %s\n", buffer);
            if (p == NULL) {
                fprintf(stderr, "Unmatched double quotes\n");
                return -1;
            }
        }
        else if (*p == '\'') {
            printf("Entering single quotes handler...\n");
            p = handle_single_quotes(buffer, p, len);
            printf("Buffer After Handling a token: %s\n", buffer);
            if (p == NULL) {
                fprintf(stderr, "Unmatched single quotes\n");
                return -1;
            }
        }
        else if (isspecial(*p)) {
            printf("Handling special character: %c\n", *p);
            p = handle_special_characters(buffer, p, len);
            printf("Buffer After Handling a token: %s\n", buffer);
        }
        else {
            while (len > 1 && *p != '\0' && !isspecial(*p) && !isspace(*p)) {
                *b++ = *p++;
                len--;
            }
            *b = '\0';
            printf("Buffer After Handling a normal token: %s\n", buffer);
        }

        printf("Extracted token: \"%s\"\n", buffer);

        if (allocateMemory(buffer, tokens, max_tokens, token_num) == NULL) {
            fprintf(stderr, "Error allocating memory\n");
            return -1;
        }
        
        for (int i = 0; i < sizeof(buffer); i++) {
            buffer[i] = 0;
        }
    }

    tokens[*token_num] = NULL; // null-terminate list
    return 0;
}


bool isspecial(char character) {
    for (int i = 0; i < SPECIALLEN; i++)
        if (character == special_characters[i])
            return true;
    return false;
}

bool is_valid_double_operator(char first, char second) {
    return (first == '>' && second == '>') ||
           (first == '<' && second == '<') ||
           (first == '&' && second == '&') ||
           (first == '|' && second == '|');
}

char *handle_single_quotes(char *argument, char *p, int max_len) {
    while (max_len > 1) {
        if (*++p == '\0') return NULL;  // prevent going out of bounds
        if (*p == '\'') break;
        *argument++ = *p;
        max_len--;
    }
    if (max_len <= 1) return NULL;
    *argument = '\0';
    return p + 1;
}

char *handle_double_quotes(char *argument, char *p, int max_len) {
    while (max_len > 1) {
        if (*++p == '\0') return NULL;
        if (*p == '\\') {
            if (*++p == '\0') return NULL;
            *argument++ = *p;
            max_len--;
        } else if (*p == '"') {
            break;
        } else {
            *argument++ = *p;
            max_len--;
        }
    }
    if (max_len <= 1) return NULL;
    *argument = '\0';
    return p + 1;
}

char *handle_special_characters(char *argument, char *p, int max_len) {
    char character = *p;
    if (max_len > 1) {
        *argument++ = character;
        max_len--;

        char *next = p + 1;
        if (*next != '\0' && is_valid_double_operator(character, *next)) {
            *argument++ = *next;
            max_len--;
            p = next;
        }

        *argument = '\0';
        return p + 1;
    }
    return NULL;
}