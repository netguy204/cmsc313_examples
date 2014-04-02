#ifndef CIRCULAR_H
#define CIRCULAR_H

/* provides size_t */
#include <stddef.h>

/*
 * This is an opaque data structure. You may define the _cbuf struct
 * inside your C file and include whatever fields you like. The user
 * of your library should never know or care about the internals of
 * your cbuf struct.
 */
struct _cbuf;
typedef struct _cbuf cbuf;

/*
 * This is the type that users will store inside your data
 * structure. Its definition is exposed to users because they will
 * need to be able to access its fields.
 *
 * Your implementation should not need to modify the provided
 * definition of this datastructure.
 */
typedef struct {
  double rate;
  unsigned int time;
} quote;


/*
 * These are the "public" functions used for interacting with your
 * datastructure. You are free to define more functions to be used as
 * internal implementation details but you should define them only in
 * your C file so that the users of your datastructure will not need
 * to know or care about them. If you wish, you can create a seperate
 * "circular_internal.h" header that has definitions of these internal
 * functions so that your tests can access them.
 */


/*
 * Initialize a circular buffer and return a pointer that can be used
 * to specify that circular buffer in future function calls.
 */
cbuf* cbuf_alloc();

/*
 * Deallocate all dynamically allocated memory associated with the
 * circular buffer referenced by cb_ptr. The programmer using your ADT
 * promises to never use the value in cb_ptr again.
 */
void cbuf_free(cbuf *cb_ptr);

/*
 * Add a new currency exchange quote to the specified circular
 * buffer. The time is in seconds since midnight. This function should
 * remove any quotes in the circular buffer that are more than 5
 * minutes older than this quote. You should assume that the
 * timestamps given to cbuf_update() are in non-decreasing
 * order. During a cbuf_update(), the memory for the circular buffer
 * may have to grow or shrink according to the rules given in the
 * project description.
 */
void cbuf_update(cbuf *cb_ptr, unsigned int time, double rate);

/*
 * Return the average exchange rate of the quotes currently stored in
 * the buffer.
 */
double cbuf_average(cbuf *cb_ptr);

/*
 * Return a pointer to the earliest quote currently stored in the
 * buffer. (Earliest = smallest timestamp) The programmer using your
 * ADT promises to just "look at" the quote and not alter it in any
 * way.
 */
const quote *cbuf_start(cbuf *cb_ptr);

/*
 * Return a pointer to the latest quote currently stored in the
 * buffer. (Latest = largest timestamp) The programmer using your ADT
 * promises to just "look at" the quote and not alter it in any way.
 */
const quote *cbuf_end(cbuf *cb_ptr);

/*
 * Return the number of elements currently stored in the circular
 * buffer.
 */
size_t cbuf_size(cbuf *cb_ptr);

/*
 * Return the current capacity of the circular buffer (will be greater
 * than or equal to the number of elements in the buffer). When
 * cbuf_size(x) == cbuf_capacity(x) then your circular buffer may need
 * to grow when the next element is inserted.
 */
size_t cbuf_capacity(cbuf *cb_ptr);

#endif
