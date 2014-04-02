/*
 * Test out circular buffer for foreign exchange quotes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circular.h"


// Print out some useful data in the given circular buffer.
//
void print_data(cbuf *cb_ptr) {
  double avg;
  avg = cbuf_average(cb_ptr);
  printf("Average rate = %f\n", avg);

  const quote *q_start, *q_end;
  q_start = cbuf_start(cb_ptr);
  q_end = cbuf_end(cb_ptr);

  printf("Start: time = %d, rate = %f\n", q_start->time, q_start->rate);
  printf("End:   time = %d, rate = %f\n", q_end->time, q_end->rate);

  return;
}


int main(int argc, char** argv) {
  cbuf *eur_usd, *eur_jpy, *usd_jpy;
  FILE *ifile;
  char* fname;
  char pair[10];
  unsigned int hour, min, isec, time;
  double sec, rate;
  int r;

  eur_usd = cbuf_alloc();
  eur_jpy = cbuf_alloc();
  usd_jpy = cbuf_alloc();

  if(argc != 2) {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }

  fname = argv[1];

  // Try to open file
  ifile = fopen (fname, "r");
  if (ifile == NULL) {
    fprintf(stderr, "Could not open file: %s\n", fname);
    exit(1);
  }


  // keep reading file until no more ticks
  //
  while(1) {

    r = fscanf(ifile, "%7s %d:%d:%lf %lf", pair, &hour, &min, &sec, &rate);
    isec = sec;

    // fprintf(stderr, "matched %d items\n", r);
    // fprintf(stderr, "%s, %02d:%02d:%02d %f\n", pair, hour, min, isec, rate);

    time = isec + 60 * min + 3600 * hour;


    if (r <= 0) break;   // EOF?

    if (strcmp(pair, "EUR-USD") == 0) {

      cbuf_update(eur_usd, time, rate);

    } else if (strcmp(pair, "EUR-JPY") == 0) {

      cbuf_update(eur_jpy, time, rate);

    } else if (strcmp(pair, "USD-JPY") == 0) {

      cbuf_update(usd_jpy, time, rate);

    } else {
      fprintf(stderr, "Not a recognized currency pair!\n");
    }

  }

  printf("\n\n*** Data for EUR-USD ***\n");
  print_data(eur_usd);

  printf("\n\n*** Data for EUR-JPY ***\n");
  print_data(eur_jpy);

  printf("\n\n*** Data for USD-JPY ***\n");
  print_data(usd_jpy);


  // Be nice, clean up
  cbuf_free(eur_usd);
  cbuf_free(eur_jpy);
  cbuf_free(usd_jpy);
  fclose(ifile) ;

  return 0;
}
