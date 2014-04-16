#include "unittest.h"

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

extern void strrev(char *str);
extern int strrepl(char *str, int c, int (* isinsubset) (int c) );

char* copy(char* str) {
  int size = strlen(str) + 1;
  char* result = malloc(size);
  memcpy(result, str, size);
  return result;
}

int main(int argc, char *argv[]) {
  char* hello = copy("Hello, World");
  strrev(hello);

  check(strcmp(hello, "Hello, World") != 0, "strrev changes the string in-place");
  check(strcmp(hello, "dlroW ,olleH") == 0, "strrev correctly reverses the string");

  char* one = copy("1");
  strrev(one);
  check(strcmp(one, "1") == 0, "a single character string is unchanged");

  char* two = copy("12");
  strrev(two);
  check(strcmp(two, "21") == 0, "a two character string is flipped");

  char* three = copy("123");
  strrev(three);
  check(strcmp(three, "321") == 0, "the middle character of a three character string stays put");

  char* the_number = copy("The number is 555-1212");
  check(strrepl(the_number, '#', &isdigit) == 7, "strrepl reports the number of replacements it made");
  check(strcmp(the_number, "The number is ###-####") == 0, "strrepl replaced the correct characters");

  char* empty = copy("");
  check(strrepl(empty, '#', &isdigit) == 0, "strrepl replaces nothing in the empty string");
  check(strcmp(empty, "") == 0, "strrepl leaves the empty string emtpy");

  strrev(NULL); // make sure we don't crash on NULL
  check(strrepl(NULL, '#', &isdigit) == 0, "strrepl replaces nothing in the NULL string");

  return check_summary();
}
