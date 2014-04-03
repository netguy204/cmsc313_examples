#include <stdio.h>
#include <stdlib.h>

typedef int Type;
typedef Type(*Mapable)(Type);
typedef Type(*Reduceable)(Type, Type);

typedef struct {
  int size;
  Type data[0];
} SizedArray;

SizedArray* array_alloc(int size) {
  SizedArray* result = (SizedArray*)malloc(sizeof(SizedArray) + sizeof(Type) * size);
  result->size = size;
  return result;
}

void array_free(SizedArray* array) {
  free(array);
}

void array_print(FILE* target, SizedArray* array) {
  fprintf(target, "[");
  for(int i = 0; i < array->size; i++) {
    if(i != 0) {
      fprintf(target, ", ");
    }
    fprintf(target, "%d", array->data[i]);
  }
  fprintf(target, "]\n");
}

SizedArray* map(SizedArray* array, Mapable mapper) {
  SizedArray* result = array_alloc(array->size);
  for(int i = 0; i < array->size; i++) {
    result->data[i] = mapper(array->data[i]);
  }
  return result;
}

Type reduce(SizedArray* array, Reduceable reducer, Type initial) {
  Type result = initial;
  for(int i = 0; i < array->size; i++) {
    result = reducer(array->data[i], result);
  }
  return result;
}

Type plus1(Type v) {
  return v + 1;
}

Type square(Type v) {
  return v * v;
}

Type add(Type v, Type sum) {
  return sum + v;
}

Type mult(Type v, Type prod) {
  return prod * v;
}

SizedArray* upto(Type max) {
  SizedArray* result = array_alloc(max + 1);
  for(int i = 0; i <= max; i++) {
    result->data[i] = i;
  }
  return result;
}

/*
 * Warning: This code leaks like a sieve.
 */
int main(int argc, char *argv[]) {
  SizedArray* array = upto(5);
  array_print(stdout, array);

  SizedArray* array_plus1 = map(array, plus1);
  array_print(stdout, array_plus1);

  Type array_plus1_sum = reduce(array_plus1, add, 0);
  printf("Sum is: %d\n", array_plus1_sum);

  SizedArray* array_plus1_sqr = map(array_plus1, square);
  array_print(stdout, array_plus1_sqr);

  Type array_plus1_sqr_prod = reduce(array_plus1_sqr, mult, 1);
  printf("Product is: %d\n", array_plus1_sqr_prod);

  return 0;
}
