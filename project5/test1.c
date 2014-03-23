#include "frac_heap.h"
#include "frac_ops.h"

#include <stdio.h>

Integer factorial(Integer k) {
  Integer r = 1;
  for(Integer i = 2; i <= k; ++i) {
    r *= i;
  }
  return r;
}

int main(int argc, char *argv[]) {
  frac_init();

  Frac* frac = frac_make(2, 6);
  frac_println(stdout, frac);

  Frac* frac2 = frac_multiply(frac_alloc(), frac, frac);
  frac_println(stdout, frac2);

  Frac* frac3 = frac_add(frac_alloc(), frac2, frac);
  frac_println(stdout, frac3);

  Frac* frac4 = frac_divide(frac_alloc(), frac3, frac);
  frac_println(stdout, frac4);

  frac_free(frac4);
  frac_free(frac3);
  frac_free(frac2);
  frac_free(frac);

  // compute a summation approximation of e^x
  int x = 2;
  Frac* e = frac_make(0, 1);
  for(int k = 0; k < 17; k++) {
    Frac* term = frac_make(ipow(x, k), factorial(k));
    frac_add(e, e, term);
    frac_free(term);
  }
  frac_print(stdout, e);
  printf(" ~= %lf\n", frac_approx(e));

  frac_free(e);

  return 0;
}
