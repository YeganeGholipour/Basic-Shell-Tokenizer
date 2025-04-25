#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXTOKENS 1000
#define MAXLEN 100
#define SPECIALLEN 5

static const char special_characters[SPECIALLEN] = {'>', '<', '&', '|', '$'};

// Function prototypes
int prompt_and_read(char **line_buffer, size_t *read);
int tokenize_line(char *line, char *tokens[], int max_tokens, int max_len,
                  int *token_num);
void print_tokens(char *tokens[], int token_num);

// MEMORY FUNCTIONS
int store_token(char *argument, char *tokens[], int max_tokens,
                int *token_num);
void freeMemory(char *tokens[], int token_num);

// HELPER FUNCTIONS
bool is_special_char(char character);
bool is_valid_double_operator(char first, char second);

// INPUT HANDLERS
char *handle_single_quotes(char token_buffer[], char *p, int max_len);
char *handle_double_quotes(char token_buffer[], char *p, int max_len);
char *handle_special_characters(char token_buffer[], char *p, int max_len);

int main(void) {
    char *tokens[MAXTOKENS];
    char *line_buffer = NULL;
    size_t read;
    int token_status, token_num, prompt_status;

    while ((prompt_status = prompt_and_read(&line_buffer, &read)) == 0) {

        if (line_buffer[read - 1] == '\n')
            line_buffer[read - 1] = '\0';

        token_num = 0;
        token_status =
            tokenize_line(line_buffer, tokens, MAXTOKENS, MAXLEN, &token_num);

        if (token_status < 0) {
            fprintf(stderr, "Error: tokenizing input\n");
            continue;
        }

        print_tokens(tokens, token_num);
        freeMemory(tokens, token_num);
    }

    if (prompt_status < 0) {
        fprintf(stderr, "Detected EOF (Ctrl+D), exiting...\n");
        return -1;
    }

    free(line_buffer);
    return 0;
}

int prompt_and_read(char **line_buffer, size_t *read) {
    size_t buffsize = 0;
    printf("****************************\n");
    printf("tokenizer> ");
    *read = getline(line_buffer, &buffsize, stdin);

    if (*read == -1)
        return -1;

    return 0;
}

void freeMemory(char *tokens[], int token_num) {
    for (int i = 0; i < token_num; i++) {
        free(tokens[i]);
        tokens[i] = NULL; 
    }
}

void print_tokens(char *tokens[], int token_num) {
    for (int i = 0; i < token_num; i++) {
        printf("%s\n", tokens[i]);
    }
}

int store_token(char *argument, char *tokens[], int max_tokens,
                int *token_num) {
    char *p;
    if (*token_num < max_tokens &&
        ((p = malloc(strlen(argument) + 1)) != NULL)) {
        strcpy(p, argument);
        tokens[(*token_num)++] = p;
    } else {
        return -1;
    }

    return 0;
}

int tokenize_line(char *line, char *tokens[], int max_tokens, int max_len,
                  int *token_num) {
    char *cursor = line;
    char token_buffer[MAXLEN] = {0};
    int memory_status;

    *token_num = 0;

    while (*cursor != '\0') {
        while (isspace(*cursor))
            cursor++;
        if (*cursor == '\0')
            break;

        char *token_pointer = token_buffer;
        int remaining_len = max_len;

        if (*cursor == '"') {
            cursor = handle_double_quotes(token_buffer, cursor, remaining_len);
            if (cursor == NULL) {
                fprintf(stderr, "Unmatched double quotes\n");
                return -1;
            }
        } else if (*cursor == '\'') {
            cursor = handle_single_quotes(token_buffer, cursor, remaining_len);
            if (cursor == NULL) {
                fprintf(stderr, "Unmatched single quotes\n");
                return -1;
            }
        } else if (is_special_char(*cursor)) {
            cursor =
                handle_special_characters(token_buffer, cursor, remaining_len);
        } else {
            while (remaining_len > 1 && *cursor != '\0' &&
                   !is_special_char(*cursor) && !isspace(*cursor)) {
                *token_pointer++ = *cursor++;
                remaining_len--;
            }
            *token_pointer = '\0';
        }

        if (store_token(token_buffer, tokens, max_tokens, token_num) == -1) {
            fprintf(stderr, "Error allocating memory\n");
            return -1;
        }
    }

    tokens[*token_num] = NULL;
    return 0;
}

bool is_special_char(char character) {
    for (int i = 0; i < SPECIALLEN; i++)
        if (character == special_characters[i])
            return true;
    return false;
}

bool is_valid_double_operator(char first, char second) {
    return (first == '>' && second == '>') || (first == '<' && second == '<') ||
           (first == '&' && second == '&') || (first == '|' && second == '|');
}

char *handle_single_quotes(char token_buffer[], char *p, int max_len) {
    while (max_len > 1) {
        if (*++p == '\0')
            return NULL;
        if (*p == '\'')
            break;
        *token_buffer++ = *p;
        max_len--;
    }
    if (max_len <= 1)
        return NULL;
    *token_buffer = '\0';
    return p + 1;
}

char *handle_double_quotes(char token_buffer[], char *p, int max_len) {
    while (max_len > 1) {
        if (*++p == '\0')
            return NULL;
        if (*p == '\\') {
            if (*++p == '\0')
                return NULL;
            *token_buffer++ = *p;
            max_len--;
        } else if (*p == '"') {
            break;
        } else {
            *token_buffer++ = *p;
            max_len--;
        }
    }
    if (max_len <= 1)
        return NULL;
    *token_buffer = '\0';
    return p + 1;
}

// Escape characters are not processed in single quotes (like in real shells)
char *handle_special_characters(char token_buffer[], char *p, int max_len) {
    char character = *p;
    if (max_len > 1) {
        *token_buffer++ = character;
        max_len--;

        char *next = p + 1;
        if (*next != '\0' && is_valid_double_operator(character, *next)) {
            *token_buffer++ = *next;
            max_len--;
            p = next;
        }

        *token_buffer = '\0';
        return p + 1;
    }
    return NULL;
}
