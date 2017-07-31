#include <stdio.h>
#include <stdlib.h>

void loop (int times) {
  while (times >= 0) {
    puts("Ya goof");
    times = times - 1;
  }
}

int main (int argc, char** argv)
    loop(atoi(argv[1]));
  } else {
    loop(10);
  }
  return 0;
}

