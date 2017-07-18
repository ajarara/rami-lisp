#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
/* #include <editline/history.h> */

int main(int argc, char** argv) {
  puts("Lispy Version 0.0.0.2");
  puts("Press C-c to exit");

  while (1) {
    char* input = readline("lispy> ");

    add_history(input);

    printf("Here's yer input: %s\n", input);

    free(input);
  }

  return 0;
}
