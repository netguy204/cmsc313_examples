#ifndef UNITTEST_H
#define UNITTEST_H

/*
 * Print the message along with a pass/fail
 */
void check(int test, const char* message);

/*
 * Print a message summarizing overall pass/fail. Return 0 on total
 * success.
 */
int check_summary();

#endif
