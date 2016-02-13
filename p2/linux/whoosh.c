#include  <fcntl.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <linux/limits.h>
#include  <sys/types.h>
#include  <sys/wait.h>

#define   LINE_LEN 128

char error_msg[30] = "An error has occurred\n";
#define error() fprintf(stdout, error_msg)
#define die() { error(); exit(1); }
#define check(A) if(!(A)) die()

int main(int argc, char *argv[]) {
    int paths = 1, itr, arg;
    char *token, *str, *slashed, buffer[PATH_MAX], line[LINE_LEN+2];
    char path[128][PATH_MAX] = { "/bin" }, *args[LINE_LEN];

    while (1) { printf("whoosh> ");
      check( fgets(line, LINE_LEN+3, stdin) );
      if (strnlen(line, LINE_LEN+2) > LINE_LEN+1) error();

      str = strtok(line, "\n"); //if (str == NULL) continue;
      token = strtok(str, " "); if (token == NULL) continue;
      if (strncmp(token, "exit", 4) == 0) exit(0);
      else if (strncmp(token, "pwd", 3) == 0) {
          check( getcwd(buffer, sizeof(buffer)) );
          printf("%s\n", buffer);
      } else if (strncmp(token, "cd", 2) == 0) {
          if ( (token = strtok(NULL, " ")) == NULL ) {
              str = getenv("HOME"); check(str);
              if (chdir(str) != 0) error();
          }
          else if (chdir(token) != 0) error();
      } else if (strncmp(token, "path", 4) == 0)
          for (paths = 0; (token = strtok(NULL, " ")) != 0; paths++)
              strncpy(path[paths], token, LINE_LEN+1);
      else {
          if (fork()) wait(NULL);
          else {
              for (itr = 0; itr < paths; itr++) {
                  slashed = strncat(strdup(path[itr]), "/", 2);
                  check( args[0] = strncat(slashed, token, LINE_LEN) );
                  if (access(args[0], X_OK) != 0) continue;
                  for (arg = 1; (token = strtok(NULL, " ")) != NULL; arg++ ) {
                      if (strncmp(token, ">", 1) == 0) {
                          if ( (token = strtok(NULL, " ")) != NULL ) {
                              close(STDOUT_FILENO);
                              check( open(token, O_CREAT | O_TRUNC | 
                                      O_WRONLY, 0644) ); 
                              arg--; break;
                          } error();
                      } else args[arg] = strndup(token, LINE_LEN);
                  }
                  args[++arg] = NULL;
                  execv(args[0], args);
              } error();
          }
      }
    } return 0;
}
