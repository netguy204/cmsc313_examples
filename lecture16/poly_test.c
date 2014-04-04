#include "poly.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  Object _;
  size_t size;
  size_t capacity;
  id* buffer;
} Array;

id array_init(id method, Array* arr) {
  object_supercall(method, arr);

  arr->size = 0;
  arr->capacity = 10;
  arr->buffer = malloc(sizeof(id) * arr->capacity);
  return arr;
}

id array_finalize(id method, Array* arr) {
  object_supercall(method, arr);

  for(size_t i = 0; i < arr->size; ++i) {
    object_call("release", arr->buffer[i]);
  }

  free(arr->buffer);
  return arr;
}

id array_push(id method, Array* arr, id value) {
  if(arr->size == arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->buffer = realloc(arr->buffer, sizeof(id) * arr->capacity);
  }

  arr->buffer[arr->size++] = object_call("retain", value);
  return value;
}

id array_element_at(id method, Array* arr, size_t idx) {
  return arr->buffer[idx];
}

id array_foreach(id method, Array* arr, void(*fn)(id)) {
  for(size_t i = 0; i < arr->size; ++i) {
    fn(arr->buffer[i]);
  }
  return arr;
}

/*
 * Define the layout of the memory associated with a Person
 * instance. We'll create the classes later.
 */
typedef struct {
  Object _;
  char name[32];
} Person;

id person_init(id method, Person* obj, const char* name) {
  object_supercall(method, obj);

  strncpy(obj->name, name, sizeof(obj->name));
  return obj;
}

id person_greet(id method, Person* obj, const char* greeter) {
  printf("Bob says, \"Hello, %s\".\n", obj->name, greeter);
  return obj;
}

id friend_greet(id method, Person* obj, const char* greeter) {
  printf("HELLO!!! %s!!!!! I'M %s!!!\n", obj->name, greeter);
  return obj;
}

/*
 * Also define a layout for a Cat
 */
typedef struct {
  Object _;
  char name[32];
  int legs;
  int alive;
} Cat;

id cat_init(id method, Cat* cat, const char* name, int legs, int alive) {
  object_supercall(method, cat);

  strncpy(cat->name, name, sizeof(cat->name));
  cat->legs = legs;
  cat->alive = alive;
  return cat;
}

id cat_greet(id method, Cat* cat, const char* greeter) {
  if(cat->alive) {
    printf("The %d legged cat named %s coldly ignores %s.\n", cat->legs, cat->name, greeter);
  } else {
    printf("%s is talking to a dead cat named %s. That's odd.\n", greeter, cat->name);
  }
  return cat;
}

int main(int argc, char *argv[]) {
  id CObject;
  oo_init(&CObject);

  id CArray = class_new(CObject, Array);
  object_call("add_method", CArray, "init", array_init);
  object_call("add_method", CArray, "finalize", array_finalize);
  object_call("add_method", CArray, "push", array_push);
  object_call("add_method", CArray, "element_at", array_element_at);
  object_call("add_method", CArray, "foreach", array_foreach);


  id CPerson = class_new(CObject, Person);
  object_call("add_method", CPerson, "init", person_init);
  object_call("add_method", CPerson, "greet", person_greet);

  id CFriend = class_new2(CPerson, Person, "Friend");
  object_call("add_method", CFriend, "greet", friend_greet);

  id CCat = class_new(CObject, Cat);
  object_call("add_method", CCat, "init", cat_init);
  object_call("add_method", CCat, "greet", cat_greet);

  id array = object_new(CArray, 1);

  id carl = object_call("push", array, object_new(CPerson, "Carl"));
  id jenny = object_call("push", array, object_new(CFriend, "Jenny"));
  id sassy = object_call("push", array, object_new(CCat, "Sassy", 4, 0));
  id tp = object_call("push", array, object_new(CCat, "Thunder Pickles", 4, 1));

  for(int ii = 0; ii < 1000; ii++) {
    object_call("push", array, tp);
  }

  // these calls are polymorphic because the behavior of the "greet"
  // method depends on the class of the object that it is being
  // invoked on.
  object_call("greet", carl, "Bob");
  object_call("greet", jenny, "Bob");
  object_call("greet", sassy, "Bob");
  object_call("greet", tp, "Bob");

  object_call("dump", jenny, stderr);
  object_call("dump", object_call("find", CObject, "Array"), stderr);
  //object_call("undefined", sassy);

  object_call("release", carl);
  object_call("release", jenny);
  object_call("release", sassy);
  object_call("release", tp);
  object_call("release", array);

  return 0;
}
