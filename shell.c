#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sh_loop(void);

char *sh_read_line(void);

char **sh_split_line(char *line);

int main(int argc, char **argv)
{

  sh_loop();

  return EXIT_SUCCESS;
}

void sh_loop(void) 
{
  char *line;
  char **args;
  int status;

    printf("> ");
    line = sh_read_line();
    args = sh_split_line(line);
                                  
    int i = 0;
    printf("Args output\n");
    while (args[i] != NULL) {
      printf("%i ", i);
      printf("%s\n", args[i]);
      ++i;
    }

    // status = sh_execute(args);

    free(line);
    free(args);
};


#define SH_RL_BUFSIZE 1024
char *sh_read_line(void)
{
  int bufsize = SH_RL_BUFSIZE;
  char *buffer = malloc(sizeof(char) * bufsize); // buffer == nullptr if there's not enough memory for allocattion.
  int position = 0;

  if (!buffer) { // is satisfied if buffer == nullptr.
      fprintf(stderr, "sh: allocation error\n");
      exit(EXIT_FAILURE);
    }

  while (1) {
    int c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    ++position;
    
    if (position >= bufsize) {
      bufsize += SH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) { // is satisfied if buffer == nullptr.
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}


#define SH_SL_BUFSIZE 10
char **sh_split_line(char *line)
{
  int bufsize = SH_SL_BUFSIZE;

  // char line[bufsize];
  char last_word[bufsize / 2];
  char **tokens = malloc(bufsize * sizeof(char*));

  size_t str_size = bufsize / 2;

  for (int i = 0; i < bufsize; ++i) {
    tokens[i] = malloc(str_size);
  }

  // Находим последнее слово в строке.
  char *p = strrchr(line, ' ');
  if (p && *(p + 1)) {
    strcpy(last_word, p + 1);
  } else { // на случай, если слово в строке одно.
    strcpy(last_word, line);
  }

  int i = 0;
  while (sscanf(line, "%s %[^\n]", tokens[i], line) > 0 && 
         strcmp(line, last_word) &&
         i < bufsize) {
    printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
    ++i;
    if (i >= bufsize) {
      printf("Bufsize exceeded");
      bufsize += SH_SL_BUFSIZE;
      tokens = realloc(tokens, bufsize);
    }
  }
  // printf("%s\n", last_word);
  printf("Exited the loop\n");
  printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
  ++i;
  // sscanf(line, "%s %[^\n]", tokens[i], line);
  tokens[i] = strdup(last_word);
  printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
  tokens[i+1] = NULL;
  // printf("%s\n", tokens[i]);

  // i = 0;
  // printf("Tokens output\n");
  // while (tokens[i] != nullptr) {
  //   printf("%i ", i);
  //   printf("%s\n", tokens[i]);
  //   ++i;
  // }

  return tokens;
}
