#include <stdio.h>

int main (int argc, char** argv) {
  puts(argv)
  loop(10);
  return 0;
}

void loop (int times) {
  while (times >= 0) {
    puts("Ya goof");
    times = times - 1;
  }
}
