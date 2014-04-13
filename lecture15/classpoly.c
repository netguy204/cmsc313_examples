#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct {
  void (*talk)();
  int how_many;
} AnimalClass;

typedef struct {
  AnimalClass* class;
  char* name;
  int fur_hairs;
} Animal;

AnimalClass* dog_class;

void bark(Animal* a) {
  printf("%s beams and proclaims \"Ruff!\"\n", a->name);
}

void mew(Animal* a) {
  printf("%s glowers and emits a chilling \"Hiss!\"\n", a->name);
}

void talk(Animal* a) {
  a->class->talk(a);
}

int main(int argc, char *argv[]) {
  AnimalClass* dog = malloc(sizeof(AnimalClass));
  dog->talk = bark;

  Animal* rover = malloc(sizeof(Animal));
  rover->class = dog;
  rover->fur_hairs = 10000000;
  rover->name = "Rover";

  talk(rover);

  free(rover);

  return 5;
}
