#include <stdio.h>

extern int add2(int a, int b);

int main(int argc, char *argv[]) {
  printf("The result is: %d\n", add2(1, 2));

  return 0;
}
