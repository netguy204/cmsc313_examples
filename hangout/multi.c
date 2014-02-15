#include <stdio.h>

unsigned int multiply(unsigned a, unsigned b) {
  int sum = 0;
  for(int i = 0; i < a; ++i) {
    sum += b;
  }
  return sum;
}

int main(int argc, char** argv) {

  unsigned int result = multiply(5, 4);
  printf("result = %u\n", result);

  return 0;
}
