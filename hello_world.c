/* including a header */
#include <stdio.h>
#include <stdlib.h>


/* 
OT: every time I think about how pretty this 
color scheme is, I think of this post:

https://groups.google.com/d/msg/golang-nuts/hJHCAaiL0so/kG3BHV6QFfIJ
*/


void talk() {
  puts("Hello world!");
};

int main(int argc, char** argv) {
  long count = atoi(argv[1]);
  for (int i=0; i < count; i++) {
    talk();
  };
  return 0;
};



