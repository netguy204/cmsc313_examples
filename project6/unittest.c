#include "unittest.h"

#include <stdio.h>

void check(int test, const char* message) {
  printf("Test `%s': %s\n", message, test ? "PASS" : "FAIL");
}
