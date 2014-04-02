/*
 * Test out circular buffer for foreign
 * exchange quotes.
 */

#include <stdio.h>
#include <stdlib.h>
#include "circular.h"

int main(int argc, char** argv) {
  cbuf *cb1 = cbuf_alloc();

  cbuf_update(cb1, 60, 1.291);
  cbuf_update(cb1, 63, 1.287);
  cbuf_update(cb1, 63, 1.231);
  cbuf_update(cb1, 69, 1.229);
  cbuf_update(cb1, 72, 1.247);


  cbuf_update(cb1, 361, 1.291);


  cbuf_update(cb1, 411, 1.291);
  cbuf_update(cb1, 412, 1.281);
  cbuf_update(cb1, 413, 1.292);
  cbuf_update(cb1, 414, 1.284);
  cbuf_update(cb1, 414, 1.290);

  cbuf_update(cb1, 511, 1.241);
  cbuf_update(cb1, 512, 1.251);
  cbuf_update(cb1, 513, 1.232);
  cbuf_update(cb1, 514, 1.202);
  cbuf_update(cb1, 517, 1.119);

  cbuf_update(cb1, 551, 1.080);
  cbuf_update(cb1, 552, 1.081);
  cbuf_update(cb1, 553, 1.079);
  cbuf_update(cb1, 554, 1.088);
  cbuf_update(cb1, 561, 1.072);
  cbuf_update(cb1, 562, 1.113);
  cbuf_update(cb1, 563, 1.091);
  cbuf_update(cb1, 564, 1.092);
  cbuf_update(cb1, 571, 1.089);
  cbuf_update(cb1, 572, 1.073);
  cbuf_update(cb1, 573, 1.061);
  cbuf_update(cb1, 574, 1.111);
  cbuf_update(cb1, 581, 1.119);
  cbuf_update(cb1, 582, 1.123);
  cbuf_update(cb1, 583, 1.151);
  cbuf_update(cb1, 584, 1.153);

  double avg;
  avg = cbuf_average(cb1);
  printf("Average rate = %f\n", avg);

  const quote *q_start, *q_end;
  q_start = cbuf_start(cb1);
  q_end = cbuf_end(cb1);

  printf("Start: time = %d, rate = %f\n", q_start->time, q_start->rate);
  printf("End:   time = %d, rate = %f\n", q_end->time, q_end->rate);

  cbuf_free(cb1);
  return 0;
}
