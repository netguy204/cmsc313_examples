#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct WordStack {
  struct WordStack* prev;
  char word[0]; // zero length array reserves no space in the struct
                // on its own so we'll have to arrange for the space
                // we need to be available after the struct.
} WordStack;

WordStack* word_push(WordStack* stack, char* word, int word_len) {
  // make enough space for the structure and the string we've placed
  // at the end of the structure (and the null character)
  WordStack* top = (WordStack*)malloc(sizeof(WordStack) + word_len);
  memcpy(top->word, word, word_len);
  top->word[word_len] = '\0'; // null terminator
  top->prev = stack;
  return top;
}

WordStack* word_pop(WordStack* top) {
  WordStack* prev = top->prev;
  free(top);
  return prev;
}

char* word_peek(WordStack* stack) {
  return stack->word;
}

int is_whitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

int is_number(char ch) {
  return ch >= '0' && ch <= '9';
}

int next_word(char** word_start, int* word_len, char* str, int str_len) {
  char* here = str;

  // skip any leading whitespace
  while(here - str < str_len && is_whitespace(*here)) {
    ++here;
  }

  // did we fall off the end?
  if(here - str == str_len) return 0;

  // mark the start of the word and move on until we find the end
  *word_start = here;
  while(here - str < str_len && !is_whitespace(*here)) {
    ++here;
  }
  *word_len = here - *word_start;

  // we found something
  return 1;
}

int precedence(char* operator) {
  if(*operator == '*') return 10;
  if(*operator == '/') return 10;
  if(*operator == '+') return 1;
  if(*operator == '-') return 1;
  return 0;
}

int main(int argc, char** argv) {
  if(argc != 2) fail_exit("usage: %s \"expression string\"", argv[0]);

  char* str = argv[1];
  int str_len = strlen(str);
  char* str_end = str + str_len;

  char* word;
  int word_len;
  WordStack* stack = NULL;

  while(next_word(&word, &word_len, str, str_len)) {
    if(is_number(*word)) {
      printf("%.*s\n", word_len, word);
    } else {
      // remove and print everything on the stack that is at a higher
      // precedence
      while(stack && precedence(word_peek(stack)) > precedence(word)) {
        printf("%s\n", word_peek(stack));
        stack = word_pop(stack);
      }
      // add the latest operator to the stack
      stack = word_push(stack, word, word_len);
    }

    // advance str and str_len
    str = word + word_len;
    str_len = str_end - str;
  }

  // pop and print all the remaining operators on the stack
  while(stack) {
    printf("%s\n", word_peek(stack));
    stack = word_pop(stack);
  }

  printf("print\n");

  // done
  return 0;
}
