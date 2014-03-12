#include <stdio.h>
#include <string.h>
#include <stdint.h>

char str[] = "Hello, World";
uint32_t nums32[] = {1, 2, 3, 4};
uint16_t nums16[] = {1, 2, 3, 4};
uint8_t nums8[]  = {1, 2, 3, 4};

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
  int a;
  Cat thunder_pickles;
  char name[64];

  thunder_pickles.num_feet = 4;
  thunder_pickles.num_fur_hairs = 1;
  thunder_pickles.teeth = 32;
  thunder_pickles.alive = 127;

  printf("thunder_pickles: name = ");
  scanf("%s", name);
  thunder_pickles.name = name;

  print_hex((uint8_t*)&thunder_pickles, sizeof(thunder_pickles));

  printf("str:\n");
  print_hex((uint8_t*)str, sizeof(str));

  printf("nums32:\n");
  print_hex((uint8_t*)nums32, sizeof(nums32));

  printf("nums16:\n");
  print_hex((uint8_t*)nums16, sizeof(nums16));

  printf("nums8:\n");
  print_hex((uint8_t*)nums8, sizeof(nums8));

  return 0;
}
