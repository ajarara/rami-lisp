#include <stdio.h>
#include <math.h>

typedef struct {
  float x;
  float y;
} point;

int main(int argc, int *argv) {
  point p;
  p.x = 0.1;
  p.y = 10.0;
  printf("%f is the sqrt!\n", sqrt(p.x * p.x + p.y + p.y));
};
  

