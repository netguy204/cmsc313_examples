#include "frac_heap.h"
#include <stdlib.h>

void frac_init() {
}

Frac* frac_alloc() {
  // WARNING: This is not an acceptable implementation of
  // alloc. Replace this according to the project description.
  return (Frac*)malloc(sizeof(Frac));
}

void frac_free(Frac* frac) {
  // WARNING: This is not an acceptable implementation of
  // free. Replace this according to the project description.
  free(frac);
}

void frac_debug() {
}
