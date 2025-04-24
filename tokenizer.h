#ifndef TOKENIZER_H
#define TOKENIZER_H

int tokenize(char *line, char *tokens[], int max_tokens, int max_len, int *token_num);
void freeMemory(char *tokens[], int token_num);

#endif