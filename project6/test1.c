/*

 * Test out circular buffer for foreign exchange quotes.
*/

#include <stdio.h>
#include <stdlib.h>
#include "circular.h"
#include "unittest.h"

int main(int argc, char** argv) {
  cbuf *cb1 = cbuf_alloc();

  // make sure the cbuf grows and shrinks appropriately
  int capacity = cbuf_capacity(cb1);
  check(capacity > 0, "Initial capacity > 0");
  check(cbuf_size(cb1) == 0, "Initial size == 0");

  for(int i = 0; i < capacity; i++) {
    cbuf_update(cb1, 60, 1.291);
  }

  check(cbuf_size(cb1) == cbuf_capacity(cb1), "Size is allowed to grow to capacity");

  cbuf_update(cb1, 60, 1.291);

  // inserting one more than what the original structure could contain
  int new_capacity = cbuf_capacity(cb1);
  check(new_capacity > capacity, "Capacity grows when necessary");

  // inserting an update that's > 5 minutes older than all of the data
  // in the structure
  cbuf_update(cb1, 60 + 6 * 60, 1.291);
  check(cbuf_capacity(cb1) < new_capacity, "Capacity shrinks when able");
  check(cbuf_size(cb1) == 1, "Aged out records are removed correctly");

  cbuf_free(cb1);

  return 0;
}
