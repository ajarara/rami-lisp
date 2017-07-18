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
  for (int i=0; i < argc; i++) {
    
    printf("%s\n", argv[i]);
    /* talk(); */
  };
  return 0;
};



