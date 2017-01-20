#include <stdio.h>

/* Declare a buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char** argv) {
  /* Print version and exit information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a loop that never ends */

  while (1) {
    /* prompt user */
    fputs("lispy> ", stdout);

    /* read in input from user */
    fgets(input, 2048, stdin);

    /* belittle user for putting it in wrong */
    printf("No, you're a %s", input);
  }
  return 0;
}
