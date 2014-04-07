#include <stdio.h>

extern int addsqrs(int x, int y);

int main(int argc, char *argv[]) {
  printf("The answer is %d\n", addsqrs(3, 4));

  return 0;
}
