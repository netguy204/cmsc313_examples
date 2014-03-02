#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*
 * Print an error message on stderr and then terminate the program
 * returning an error code.
 */
void fail_exit(const char * message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);

  fprintf(stderr, "\n");
  fflush(stderr);
  exit(1);
}


/*
 * Very simple implementation of a stack that can grow to any size.
 */
typedef struct {
  int* values;
  int top;
  int max;
} IntStack;

void stack_init(IntStack* stack) {
  // our default stack size is large enough for 16 elements
  stack->max = 16;
  stack->values = (int*)malloc(sizeof(int) * stack->max);
  stack->top = 0;
}

/*
 * Push an element onto the stack, growing the stack if necessary. The
 * stack grows towards increasing indices.
 */
void stack_push(IntStack* stack, int value) {
  if(stack->top == stack->max) {
    // need to grow our stack. we just double its size.
    stack->max = stack->max * 2;
    stack->values = (int*)realloc(stack->values, sizeof(int) * stack->max);
  }
  stack->values[stack->top] = value;
  stack->top = stack->top + 1;
}

/*
 * Pop an element by making its position available. Return the element
 * that was popped.
 */
int stack_pop(IntStack* stack) {
  if(stack->top == 0) fail_exit("can't pop an empty stack");
  stack->top = stack->top - 1;
  return stack->values[stack->top];
}

/*
 * Return the element at the top of the stack.
 */
int stack_peek(IntStack* stack) {
  if(stack->top == 0) fail_exit("can't peek an empty stack");
  return stack->values[stack->top - 1];
}
/*
 * end of simple stack implementation
 */



/*
 * Convert a value to a string in any base (binary, decimal,
 * hexadecimal). Hm. Could this be helpful for project 3?
 */

// static buffer that base_str fills with data and then returns. What
// are the implications of using the same buffer each time base_str is
// called? What could we do instead?
char num_buffer[33];

char* base_str(int value, int base) {
  int isNegative = 0;
  if(value < 0) {
    isNegative = 1;
    value = -value;
  }

  num_buffer[sizeof(num_buffer - 1)] = '\0'; // terminate the string
  int position = sizeof(num_buffer) - 2; // next available position

  if(value == 0) {
    num_buffer[position--] = '0';
  } else {
    while(value > 0) {
      int digit = value % base;
      value = value / base;

      if(digit > 9) {
        num_buffer[position--] = 'A' + (digit - 10);
      } else {
        num_buffer[position--] = '0' + digit;
      }
    }
  }

  if(isNegative) {
    num_buffer[position--] = '-';
  }

  return &num_buffer[position + 1];
}

/*
 * Read a number in any base. Calls fail exit if the number is not a
 * valid number in the given base.
 */
int read_base(const char* str, int base) {
  // start decoding at the end of the string
  int position = strlen(str);
  int total = 0;
  int place_value = 1;

  if(position == 0) {
    fail_exit("cannot read empty string");
  }

  do {
    --position;
    int value = base; // initial value will trigger our error check

    if(str[position] >= '0' && str[position] <= '9') {
      // convert a numeric digit
      value = str[position] - '0';
    } else if(str[position] >= 'A' && str[position] <= 'Z') {
      // convert an alpha digit
      value = 10 + (str[position] - 'A');
    } else if(str[position] >= 'a' && str[position] <= 'z') {
      // handle lower case as well
      value = 10 + (str[position] - 'a');
    }

    if(value >= base) {
      fail_exit("'%c' is not a valid digit in base %d", str[position], base);
    }
    total += (value * place_value);
    place_value *= base;
  } while(&str[position] > str);

  return total;
}

int main(int argc, char *argv[]) {
  // we need at least 1 argument to do anything useful
  if(argc < 2) fail_exit("usage: %s operation_1 [operations_2 .. operation_n]", argv[0]);

  // default base is 10 (decimal)
  int base = 10;

  IntStack stack;
  stack_init(&stack);

  for(int ii = 1; ii < argc; ++ii) {
    // dispatch each possible operation
    if(strcmp(argv[ii], "set-base") == 0) {
      // set the current base to the value at the top of the stack
      base = stack_pop(&stack);
      if(base < 1) fail_exit("invalid base '%d'. Base must be greater than 0", base);
    } else if(strcmp(argv[ii], "+") == 0) {
      // addition
      int b = stack_pop(&stack);
      int a = stack_pop(&stack);
      int result = a + b;
      stack_push(&stack, result);
    } else if(strcmp(argv[ii], "-") == 0) {
      // subtraction
      int b = stack_pop(&stack);
      int a = stack_pop(&stack);
      int result = a - b;
      stack_push(&stack, result);
    } else if(strcmp(argv[ii], "/") == 0) {
      // division
      int b = stack_pop(&stack);
      int a = stack_pop(&stack);
      int result = a / b;
      stack_push(&stack, result);
    } else if(strcmp(argv[ii], "*") == 0) {
      // multiplication
      int b = stack_pop(&stack);
      int a = stack_pop(&stack);
      int result = a * b;
      stack_push(&stack, result);
    } else if(strcmp(argv[ii], "^") == 0) {
      // exponentiation
      int b = stack_pop(&stack);
      int a = stack_pop(&stack);
      int result = 0;
      for(int jj = 0; jj < b; ++jj) {
        if(result == 0) {
          result = a;
        } else {
          result *= a;
        }
      }
      stack_push(&stack, result);
    } else if(strcmp(argv[ii], "print") == 0) {
      // pop and print the value on the top of the stack
      int value = stack_pop(&stack);
      printf("%s\n", base_str(value, base));
    } else {
      // assume we have a number in BASE
      int value = read_base(argv[ii], base);
      stack_push(&stack, value);
    }
  }

  return 0;
}
