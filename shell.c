#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sh_loop(void);
char *sh_read_line(void);
char **sh_split_line(char *line);
int len(char *str);
int word_count(char *line);
int max_word_len(char *line);

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

  while (args[i] != NULL) {
    free(args[i]);
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

int len(char *str) 
{
  int length = 0;
  while (str[length]) {
    ++length;
  }
  return length;
}

int max_word_len(char *line)
{
  int line_length = len(line);
  int max_length = 0;
  if (line_length == 0) {
    return 0;
  } else { 
    int i = 0;
    int cur_length = 0;
    while (i <= line_length) {
      if ((32 <= line[i] && line[i] <= 47) || !line[i]) {
        if (cur_length > max_length) {
          max_length = cur_length;
        }
        cur_length = 0;
      } else {
        ++cur_length;
      }
      ++i;
    }
    return max_length;
  }
  return 0;
}

int word_count(char *line)
{
  int line_length = len(line);
  if (line_length == 0) {
    return 0;
  } else { 
    int word_count = 1;
    int i = 0;
    while (i < line_length) {
      if (line[i] == 32) {
        ++word_count;
      }
      ++i;
    }
    return word_count;
  }
  return 0;
}

char **sh_split_line(char *line)
{
  int line_length = len(line);
  int line_word_count = word_count(line);
  // printf("%i\n", line_length);
  int max_word_length = max_word_len(line);

  char **tokens = malloc((line_word_count + 1) * sizeof(char*)); // add one for null terminator.
  // char *last_word = malloc(max_word_length * sizeof(char));

  // for (int i = 0; i < line_length; ++i) {
  //   tokens[i] = malloc(len(line[i]) * sizeof(char));
  // }

  // // Находим последнее слово в строке.
  // char *p = strrchr(line, ' ');
  // if (p && *(p + 1)) {
  //   strcpy(last_word, p + 1);
  // } else { // на случай, если слово в строке одно.
  //   strcpy(last_word, line);
  // }

  int i = 0, j = 0;
  char current_word[max_word_length];
  int current_word_length = 0;
  int word_count = 0;
  while (i <= line_length) {
    if ((32 <= line[i] && line[i] <= 47) || !line[i]) {
      tokens[word_count] = malloc(current_word_length * sizeof(char));
      // printf("Current word: %s\n", current_word);
      strncpy(tokens[word_count], current_word, current_word_length);
      tokens[word_count][current_word_length] = '\0';
      printf("tokens["); printf("%i", word_count); printf("]: %s\n", tokens[word_count]);
      ++word_count; 
      current_word_length = 0;
      j = 0;
    } else {
      current_word[j] = line[i];
      ++current_word_length;
      ++j;
    }
    ++i;
  }
  //printf("tokens["); printf("%i", word_count); printf("]: %s\n", tokens[word_count]);
  tokens[i] = NULL;

  /*
  while (i < line_length) {
    sscanf(line, "%s %[^\n]", tokens[i], line);
    printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
    ++i;
    if (i >= line_length) {
      printf("Bufsize exceeded\n");
      line_length += SH_SL_LINE_LENGTH;
      tokens = realloc(tokens, line_length * sizeof(char*));
      for (int j = i; j < line_length; ++j) {
        tokens[j] = malloc(word_length);
      }
      if (!tokens) { // is satisfied if tokens == nullptr.
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  */
  // printf("Exited the loop\n");
  // printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
  // ++i;
  // // sscanf(line, "%s %[^\n]", tokens[i], line);
  // tokens[i] = last_word;
  // printf("tokens["); printf("%i", i); printf("]: %s\n", tokens[i]);
  // printf("%s\n", tokens[i]);

  return tokens;
}
