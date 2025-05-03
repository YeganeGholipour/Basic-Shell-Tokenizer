#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

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