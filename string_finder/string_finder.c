#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

int is_string_part(int ch) {
  return (ch == ' ' || isgraph(ch));
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    fprintf(stderr, "usage: %s size_in_bytes min_string_size\n", argv[0]);
    exit(1);
  }

  size_t min_size = atol(argv[2]);
  char* mem;
  size_t size;

  // try taking the first arg as a file. If that doesn't work then
  // just treat it as a number.
  FILE* f = fopen(argv[1], "r");
  if(!f) {
    const char* fname = argv[1];
    fprintf(stderr, "Failed to open `%s'. Assuming heap request.\n", fname);
    size = atol(argv[1]);
    mem = malloc(size);
  } else {
    struct stat sb;
    stat(argv[1], &sb);

    size = sb.st_size;
    mem = malloc(size);
    fread(mem, size, 1, f);
    fclose(f);
  }

  // hunt for strings
  for(size_t i = 0; i < size; ++i) {
    for(size_t j = 0; j < (size - i); ++j) {
      char ch = mem[i + j];
      if(!is_string_part(ch)) {
        if(ch == '\0' && j > min_size) {
          printf("%lu@%lu : `%s'\n", i, j, &mem[i]);
        }
        i = i + j;
        break;
      }
    }
  }

  free(mem);
  return 0;
}
