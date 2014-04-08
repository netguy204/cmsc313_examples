#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

int is_string_part(int ch) {
  return (ch == ' ' || isgraph(ch));
}

void show_strings(char* mem, size_t size, size_t min_size) {
  // hunt for strings
  for(size_t i = 0; i < size; ++i) {
    for(size_t j = 0; j < (size - i); ++j) {
      char ch = mem[i + j];
      if(!is_string_part(ch)) {
        if(ch == '\0' && j >= min_size) {
          printf("%08x : `%s'\n", i, &mem[i]);
        }
        i = i + j;
        break;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    fprintf(stderr, "usage: %s size_in_bytes min_string_size\n", argv[0]);
    exit(1);
  }

  size_t min_size = atol(argv[2]);

  // try taking the first arg as a file. If that doesn't work then
  // just treat it as a number.
  const char* fname = argv[1];
  FILE* f = fopen(fname, "r");
  if(f) {
    struct stat sb;
    stat(fname, &sb);
    size_t size = sb.st_size;
    char* mem = malloc(size);
    fread(mem, size, 1, f);
    fclose(f);

    show_strings(mem, size, min_size);
    free(mem);

  } else {
    fprintf(stderr, "Failed to open `%s'. Assuming proc request.\n", fname);

    // see if we can find that process
    char memtable_name[64];
    char memfile_name[64];

    snprintf(memtable_name, sizeof(memtable_name), "/proc/%s/maps", argv[1]);
    snprintf(memfile_name, sizeof(memfile_name), "/proc/%s/mem", argv[1]);

    FILE* memtable = fopen(memtable_name, "r");
    if(!memtable) {
      fprintf(stderr, "Failed to open `%s'. Don't know what to do with request.\n",
              memtable_name);
      exit(1);
    }

    FILE* memfile = fopen(memfile_name, "r");
    if(!memfile) {
      fprintf(stderr, "Couldn't open `%s'. Do you have permissions?\n", memfile_name);
      exit(1);
    }

    char line[256];
    while(fgets(line, sizeof(line), memtable)) {
      size_t start;
      size_t end;
      char p;

      if(sscanf(line, "%08x-%08x %c", &start, &end, &p) == 3) {
        size_t size = end - start;

        printf("Searching %08x-%08x (%lu bytes)\n", start, end, size);
        char* mem = malloc(size);
        fseek(memfile, start, SEEK_SET);
        fread(mem, size, 1, memfile);
        show_strings(mem, size, min_size);
        free(mem);
      }
    }

    fclose(memfile);
    fclose(memtable);
  }


  return 0;
}
