#include  <fcntl.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <sys/wait.h>

#define LINE_LEN 128
#define error() fprintf(stderr, "An error has occurred\n")
#define die() { error(); exit(1); }
#define check(A) if(!(A)) die()
#define open_std(A, B, C) if (dup2(open(strncat(strdup(A), B, 5), \
                            O_CREAT | O_TRUNC | O_WRONLY, 0644), C) < 0) die();

int main(int argc, char *argv[]) {
    int paths = 2, itr, arg;
    char *token, *str, buffer[4097], line[LINE_LEN+2];
    char path[129][4097] = { "", "/bin/" }, *args[LINE_LEN];

    if (argc > 1) die();  // Bad args
    setbuf(stdout, NULL); // Don't buffer

    while (1) { printf("whoosh> "); // Main loop
      check( fgets(line, LINE_LEN+3, stdin) ); // Get the next line, check len
      if (strnlen(line, LINE_LEN+2) > LINE_LEN+1) {
          while ( (buffer[0] = fgetc(stdin)) != '\n' && buffer[0] != EOF);
          error(); continue; // move on after dumping the remaining characters
      } str = strtok(line, "\n"); if (str == NULL) continue; // is line bad
      token = strtok(str, " "); if (token == NULL) continue; // is line blank

      if (strncmp(token, "exit", 4) == 0) exit(0);      // exit
      else if (strncmp(token, "pwd", 3) == 0) {         // pwd
          check( getcwd(buffer, sizeof(buffer)) );
          printf("%s\n", buffer);
      } else if (strncmp(token, "cd", 2) == 0) {        // cd
          if ( (token = strtok(NULL, " ")) == NULL ) {  //    cd no args
              if (chdir(getenv("HOME")) != 0) error();
          } else if (chdir(token) != 0) error();        //    cd path
      } else if (strncmp(token, "path", 4) == 0) {      // path
          for (paths = 1; (token = strtok(NULL, " ")) != 0; paths++)
              strncpy(path[paths], strncat(strdup(token), "/", 2), LINE_LEN);
      } else {                                          // external cmd
          if (fork()) wait(NULL);   // parent wait for child
          else {                    // child
              args[0] = strdup(token); // save the command and build its args
              for (arg = 1; (token = strtok(NULL, " ")) != NULL; arg++ ) {
                  if (strncmp(token, ">", 1) == 0) { // is this a redirect
                      if ( (token = strtok(NULL, " ")) != NULL ) { // file
                          if (strtok(NULL, " ") != NULL) die(); // no more args
                          open_std(token, ".out", STDOUT_FILENO);
                          open_std(token, ".err", STDERR_FILENO);
                          arg--; break; // redirect must be the last arg
                      } die();
                  } else args[arg] = strndup(token, LINE_LEN); // add arg
              } args[++arg] = NULL; // make sure last arg is NULL

              for (itr = 0; itr < paths; itr++) { // try and exec the cmd 
                  check( str = strncat(strdup(path[itr]), args[0], LINE_LEN) );
                  if (itr == 0 && args[0][0] != '/') continue; // no exec wd
                  else if (access(str, X_OK) != 0) continue; // check perms
                  else execv(args[0] = str, args); die();
              } die(); // die so parent can continue
} /* child */ } /* ext cmd */ } /* mainloop */ return 0; } /* main */
