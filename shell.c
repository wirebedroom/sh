// TODO
// Maybe use a linked list for args.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void sh_loop(void);
char *sh_read_line(void);
char **sh_split_line(char *line);
void sh_launch(char **args);
int sh_word_count(char *line);
int sh_max_word_len(char *line);

int main(int argc, char **argv)
{

  sh_loop();

  return EXIT_SUCCESS;
}

void sh_loop(void) 
{
  char *line;
  char **args;
  int status = 1;

  do {
    printf("> ");
    line = sh_read_line();
    args = sh_split_line(line);
                                  
    // printf("Args output\n");
    // int i = 0;
    // while (args[i] != NULL) {
    //   printf("%i ", i);
    //   printf("%s\n", args[i]);
    //   ++i;
    // }

    sh_launch(args);

    int i = 0;
    while (args[i] != NULL) {
      free(args[i]);
      ++i;
    }

    // status = sh_execute(args);

    free(line);
    free(args);
  } while (status);
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

int sh_max_word_len(char *line)
{
  int line_length = strlen(line);
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

int sh_word_count(char *line)
{
  int line_length = strlen(line);
  if (line_length == 0) {
    return 0;
  } else { 
    int word_count = 1;
    int i = 0;
    while (i < line_length) {
      if (32 <= line[i] && line[i] <= 47) {
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
  int line_length = strlen(line);
  int line_word_count = sh_word_count(line);
  int max_word_length = sh_max_word_len(line);

  char **tokens = malloc((line_word_count + 1) * sizeof(char*)); // add one for null terminator.

  int i = 0, j = 0;
  char current_word[max_word_length];
  int current_word_length = 0;
  int word_count = 0;
  bool is_word = false;
  while (i <= line_length) {
    if ( ((32 <= line[i] && line[i] <= 47) || !line[i]) && is_word ) { // if the previous symbol was not part of a word (it was a dot, a comma, a space, etc), then we don't write it to tokens.
      current_word[j] = '\0';
      tokens[word_count] = malloc(current_word_length * sizeof(char));
      strncpy(tokens[word_count], current_word, current_word_length);
      tokens[word_count][current_word_length] = '\0';
      // printf("tokens["); printf("%i", word_count); printf("]: %s\n", tokens[word_count]);
      ++word_count; 
      current_word_length = 0;
      j = 0;
      is_word = false;
    } else if ( !((32 <= line[i] && line[i] <= 47) || 
                  !line[i]) ) {
      current_word[j] = line[i];
      ++current_word_length;
      ++j;
      is_word = true; // we have now began reading a proper word.
    }
    ++i;
  }
  tokens[word_count] = NULL;

  return tokens;
}


void sh_launch(char **args)
{
  pid_t pid = fork();
  pid_t wpid;
  int status;

  if (pid == 0) { // go into the child process.
    if (execvp(args[0], args) == -1) {
      perror("sh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("sh");
  } else { // go into the parent process. pid always has the parent process first.
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
      if (wpid == -1) {
        perror("sh");
        break;
      }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status)); // while the child process hasn't exited/been stopped.
  }
  return;
}
