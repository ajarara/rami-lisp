#include <stdio.h>

static char input[2048];

int main(int argc, char** argv) {
  puts("Lispy version 2.7.1.828");
  puts("C-c to Exit\n");
  while (1) {
    fputs("lispy> ", stdout);
    fgets(input, 2048, stdin);
    printf("Primitive eval? You're the primitive one!\n");
  }
  return 0;
}
