/*
 * Q1: What function below would be undeclared if we had failed to
 * include stdio.h?
 *
 * Q2: Why do we use <stdio.h> instead of "stdio.h"?
 */
#include <stdio.h>

/*
 * Count the number times that a particular character appears in the
 * supplied string.
 *
 * Q3: Why is haystack a pointer to char while needle is a char?
 *
 * Q4: What is a major deficiency of this function when we consider
 *     certain major alphabets other than the latin alphabet? How
 *     would you correct this problem?
 */
int char_count(char* haystack, char needle) {
  int count = 0;

  /*
   * In the for loop syntax:
   * Q5: Why is str dereferenced (*str) in the test?
   * Q6: Why are we looking for the null character ('\0') in the test?
   * Q7: Why do we increment str after each pass through the loop?
   */
  for(char* str = haystack; *str != '\0'; str++) {
    /*
     * Q8: Why is str dereferenced (*str) in this test?
     */
    if(*str == needle) {
      count++;
    }
  }

  return count;
}

int main(int argc, char *argv[]) {
  /*
   * BONUS: Who is often credited with this quote?
   */
  char msg[] =
    "Everything should be made as simple as "
    "possible, but not simpler.";


  printf("Number of o's: %d\n", char_count(msg, 'o'));

  /*
   * Q9: How many NULL characters will be found?
   */
  printf("Number of NULL characters: %d\n", char_count(msg, '\0'));

  return 0;
}
