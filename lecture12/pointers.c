#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
  printf("argv:\n");
  print_hex((uint8_t*)argv, 30);

  for(int cc = 0; cc < argc; ++cc) {
    printf("argv[%d]:\n", cc);
    print_hex((uint8_t*)argv[cc], strlen(argv[cc]) + 1);
  }

  return 0;
}
