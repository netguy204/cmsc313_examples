#include <stdio.h>
#include <stdint.h>

typedef struct {
  int num_feet;
  char *name;
  int num_fur_hairs;
  int teeth;
  char alive;
} Cat;


void print_hex(uint8_t* data, size_t count) {
  size_t row_count = 8;

  for(size_t byte = 0; byte < count; ++byte) {
    if(byte % row_count == 0) {
      if(byte != 0) printf("\n");
      printf("0x%08x:", byte / row_count);
    }
    printf("  0x%02x", data[byte]);
  }
  printf("\n");
}


int main(int argc, char *argv[]) {
  Cat thunder_pickles = {4, "Thunder Pickles", 42, 10, 33};

  print_hex((uint8_t*)&print_hex, 40);

  return 0;
}
