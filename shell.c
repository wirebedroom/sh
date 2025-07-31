// A simple shell. Line symbols are whitelisted.
// I'm actually not sure if we need to handle stuff inside the quotation marks as a single argument. echo works fine, I don't know of any other use cases for quotation marks. For now I'm just going to ignore them.
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
    const char *user = getenv("USER");
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    const char *pwd = getenv("PWD");

    printf("[%s@%s %s]$ ", user, hostname, pwd);
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
      fprintf(stderr, "wsh: allocation error\n");
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
        fprintf(stderr, "wsh: allocation error\n");
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
  printf("Line length: %i\n", line_length);
  int line_word_count = sh_word_count(line);
  int max_word_length = sh_max_word_len(line) + 2; // + 1 for null termination and + 1 for a possible space.

  char **tokens = malloc((line_word_count + 1) * sizeof(char*)); // add one for null terminator.

  int i = 0, j = 0;
  char current_word[max_word_length];
  int current_word_length = 0;
  int word_count = 0;
  bool is_word = false;
  while (i <= line_length) {
    // Handling quotation marks. Two pointers method.
    // We can't treat the thing inside the quotation marks as one word because max_word_len doesn't care about that.
    // We have enough space for the longest word and no more.
    if (i + 1 < line_length && line[i] == '"') {
      int r = i + 1;
      j = 0;
      while (r < line_length && line[r] != '"' && j < max_word_length - 1 && // - 1 because '\0'.
             32 <= line[r] && line[r] < 127) { // 32, so spaces are allowed.
        current_word[j] = line[r];
        ++current_word_length;
        ++j;
        ++r;
        is_word = true; // we have now begun reading a proper word.
      }
      i = r + 1; // step off the closing quotation mark.
    }
    // Handling other cases.
    if ((0 <= line[i] && line[i] < 33) && is_word) { // if the previous symbol was SPACE or TAB or ESC or etc, then we don't write it to tokens.
      current_word[j] = '\0';
      tokens[word_count] = malloc(current_word_length * sizeof(char));
      strncpy(tokens[word_count], current_word, current_word_length);
      tokens[word_count][current_word_length] = '\0';
      ++word_count; 
      current_word_length = 0;
      j = 0;
      is_word = false;
    } else if (33 <= line[i] && line[i] < 127) { // the upper bound is 127 because 127 == 'del'.
      current_word[j] = line[i];
      ++current_word_length;
      ++j;
      is_word = true; // we have now begun reading a proper word.
    }    
    ++i;
  }
  tokens[word_count] = NULL;

  return tokens;
}


void sh_launch(char **args)
{
  if (!strcmp(args[0], "cd")) { // handling cd. Also will have to handle other builtins.
    if (chdir(args[1])) {
      perror("wsh");
    }
    setenv("PWD", args[1], 1);
  } else {
    pid_t pid = fork();
    pid_t wpid;
    int status;

    if (pid == 0) { // go into the child process.
      if (execvp(args[0], args) == -1) {
        perror("wsh");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("wsh");
    } else { // go into the parent process. pid always has the parent process first.
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
        if (wpid == -1) {
          perror("wsh");
          break;
        }
      } while (!WIFEXITED(status) && !WIFSIGNALED(status)); // while the child process hasn't exited/been stopped.
    }
  }
  return;
}
