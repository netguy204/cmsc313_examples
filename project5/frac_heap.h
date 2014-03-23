#ifndef FRAC_HEAP_H
#define FRAC_HEAP_H

#include <stdint.h>

/*
 * the representation we use for the numerator and
 * denominator. different choices can be made here to trade size of
 * the Frac against its ability to represent more values.
 */
typedef int64_t Integer;

/*
 * the format character that printf should use to print whatever type
 * we've chosen.
 */
#define IntegerFormat "%lld"

typedef struct {
  Integer numerator;
  Integer denominator;
} Frac;

/*
 * must be called once by the program using your functions before
 * calls to any other functions are made. This allows you to set up
 * any housekeeping needed for your memory allocator. For example,
 * this is when you can initialize your linked list of free blocks.
 */
void frac_init();

/*
 * must return a pointer to fraction. This must be the address of an
 * item in your global array of fractions. It should be an item taken
 * from the list of free blocks. (Don't forget to remove it from the
 * list of free blocks!)
 */
Frac* frac_alloc();

/*
 * takes a pointer to fraction and adds that item to the free block
 * list. The programmer using your functions promises to never use
 * that item again, unless the item is given to her/him by a
 * subsequent call tonew_frac().
 */
void frac_free(Frac* frac);

/*
 * is for debugging/diagnostic purposes. It should print out the
 * entire contents of the global array and the head of the free block
 * list. This allows you to see how your memory allocator is working.
 */
void frac_debug();

#endif
