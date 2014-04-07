#include "unittest.h"

#include <stdio.h>

static int tests = 0;
static int passes = 0;

void check(int test, const char* message) {
  tests++;
  if(test) passes++;

  printf("[Test %d] `%s': %s\n", tests, message, test ? "PASS" : "FAIL");
}

int check_summary() {
  if(tests == passes) {
    printf("All Tests Pass!\n");
    return 0;
  } else {
    printf("%d out of %d tests did not pass.\n", tests-passes, tests);
    return 1;
  }
}
