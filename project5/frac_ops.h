#ifndef FRAC_OPS_H
#define FRAC_OPS_H

#include "frac_heap.h"
#include <stdio.h>

/*
 * allocate a new frac and initialize it with the given numerator and
 * denominator
 */
Frac* frac_make(Integer numerator, Integer denominator);

/*
 * print a frac to the provide stream
 */
void frac_print(FILE* output, Frac* frac);

/*
 * print a frac, followed by a newline
 */
void frac_println(FILE* output, Frac* frac);

/*
 * approximate a frac as a double
 */
double frac_approx(Frac* frac);

/*
 * reduces the supplied fraction to its simplest form. mutates the
 * supplied fraction and returns it. does not allocate.
 */
Frac* frac_reduce(Frac* frac);

/*
 * add two fractions, place the result in the supplied pointer. does
 * not allocate.
 */
Frac* frac_add(Frac* result, Frac* a, Frac* b);

/*
 * subtract two fractions, place the result in the supplied
 * pointer. does not allocate.
 */
Frac* frac_subtract(Frac* result, Frac* a, Frac* b);

/*
 * multiply two fractions, place the result in the supplied
 * pointer. does not allocate.
 */
Frac* frac_multiply(Frac* result, Frac* a, Frac* b);

/*
 * divide two fractions, place the result in the supplied
 * pointer. does not allocate.
 */
Frac* frac_divide(Frac* result, Frac* a, Frac* b);

/*
 * raise a fraction to a power, place the result in the supplied
 * pointer. does not allocate
 */
Frac* frac_pow(Frac* result, Frac* a, Integer pow);

// not really part of frac_ops but a useful utility that needed to be
// available to the test programs as well.
Integer ipow(Integer base, Integer exp);

#endif
