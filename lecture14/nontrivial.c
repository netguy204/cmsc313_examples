#include <stdio.h>

int sumsqrs(int n);

int main(int argc, char *argv[]) {
  printf("sumsqrs(3) = %d, sumsqrs(6) = %d, sumsqrs(9) = %d\n",
         sumsqrs(3), sumsqrs(6), sumsqrs(9));

  return 0;
}
