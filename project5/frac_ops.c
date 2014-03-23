#include "frac_heap.h"
#include "frac_ops.h"

#include <math.h>
#include <stdlib.h>

#define ABS(x) (x > 0 ? (x) : -(x))

// private: greatest-common-divisor
static Integer gcd(Integer a, Integer b) {
  while(b != 0) {
    Integer temp = b;
    b = a - b * (a / b);
    a = temp;
  }

  return ABS(a);
}

Frac* frac_make(Integer numerator, Integer denominator) {
  Frac* frac = frac_alloc();
  frac->numerator = numerator;
  frac->denominator = denominator;
  return frac_reduce(frac);
}

void frac_print(FILE* output, Frac* frac) {
  if(frac->numerator == 0) {
    fprintf(output, "0");
  } else {
    fprintf(output, IntegerFormat "/" IntegerFormat,
            frac->numerator, frac->denominator);
  }
}

void frac_println(FILE* output, Frac* frac) {
  frac_print(output, frac);
  fprintf(output, "\n");
}

double frac_approx(Frac* frac) {
  return (double)frac->numerator / (double)frac->denominator;
}

Frac* frac_reduce(Frac* frac) {
  Integer common = gcd(frac->numerator, frac->denominator);
  frac->numerator /= common;
  frac->denominator /= common;

  if(frac->denominator < 0) {
    frac->numerator *= -1;
    frac->denominator *= -1;
  }

  return frac;
}

Frac* frac_add(Frac* result, Frac* a, Frac* b) {
  Integer num = a->numerator * b->denominator + b->numerator * a->denominator;
  Integer den = a->denominator * b->denominator;
  result->numerator = num;
  result->denominator = den;
  return frac_reduce(result);
}

Frac* frac_subtract(Frac* result, Frac* a, Frac* b) {
  Integer num = a->numerator * b->denominator - b->numerator * a->denominator;;
  Integer den = a->denominator * b->denominator;;
  result->numerator = num;
  result->denominator = den;
  return frac_reduce(result);
}

Frac* frac_multiply(Frac* result, Frac* a, Frac* b) {
  Integer num = a->numerator * b->numerator;;
  Integer den = a->denominator * b->denominator;;
  result->numerator = num;
  result->denominator = den;
  return frac_reduce(result);
}

Frac* frac_divide(Frac* result, Frac* a, Frac* b) {
  Integer num = a->numerator * b->denominator;;
  Integer den = a->denominator * b->numerator;;
  result->numerator = num;
  result->denominator = den;
  return frac_reduce(result);
}

Frac* frac_pow(Frac* result, Frac* a, Integer pow) {
  result->numerator = ipow(a->numerator, pow);
  result->denominator = ipow(a->denominator, pow);
  return frac_reduce(result);
}

// from stackoverflow:
// http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
Integer ipow(Integer base, Integer exp) {
  Integer result = 1;
  while(exp) {
    if (exp & 1) {
      result *= base;
    }
    exp >>= 1;
    base *= base;
  }
  return result;
}
