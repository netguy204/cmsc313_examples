#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct {
  void (*talk)();
  char name[32];
} Animal;

void bark(Animal* a) {
  printf("%s beams and proclaims \"Ruff!\"\n", a->name);
}

void mew(Animal* a) {
  printf("%s glowers and emits a chilling \"Hiss!\"\n", a->name);
}

Animal* animal_make(const char* name, void(*talk)()) {
  Animal* a = malloc(sizeof(Animal));
  a->talk = talk;
  strncpy(a->name, name, sizeof(a->name));
  return a;
}

Animal* dog(const char* name) {
  return animal_make(name, bark);
}

Animal* cat(const char* name) {
  return animal_make(name, mew);
}

void talk(Animal* a) {
  a->talk(a);
}

int main(int argc, char *argv[]) {
  Animal* rover = dog("Rover");
  Animal* sassy = cat("Sassy");

  talk(rover);
  talk(sassy);

  Animal buddy;
  buddy.talk = bark;
  buddy.name[0] = 'B';
  buddy.name[1] = '\0';

  buddy.talk(&buddy);

  free(rover);
  free(sassy);

  return 0;
}
