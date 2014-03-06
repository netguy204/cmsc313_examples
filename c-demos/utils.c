#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

/*
 * Print an error message on stderr and then terminate the program
 * returning an error code.
 */
void fail_exit(const char * message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);

  fprintf(stderr, "\n");
  fflush(stderr);
  exit(1);
}
