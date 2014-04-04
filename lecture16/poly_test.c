#include "poly.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Define the layout of the memory associated with a Person
 * instance. We'll create the classes later.
 */
struct Person {
  char name[32];
};

id person_init(id method, struct Person* obj, const char* name) {
  object_supercall(method, obj);

  strncpy(obj->name, name, sizeof(obj->name));
  return obj;
}

id person_greet(id method, struct Person* obj) {
  printf("\"Hello, %s\".\n", obj->name);
  return obj;
}

id friend_greet(id method, struct Person* obj) {
  printf("\"HELLO!!! %s!!!!\"\n", obj->name);
  return obj;
}

/*
 * Also define a layout for a Cat
 */
struct Cat {
  char name[32];
  int legs;
  int alive;
};

id cat_init(id method, struct Cat* cat, const char* name, int legs, int alive) {
  object_supercall(method, cat);

  strncpy(cat->name, name, sizeof(cat->name));
  cat->legs = legs;
  cat->alive = alive;
  return cat;
}

id cat_greet(id method, struct Cat* cat) {
  if(cat->alive) {
    printf("The %d legged cat named %s coldly ignores you.\n", cat->legs, cat->name);
  } else {
    printf("You are talking to a dead cat named %s. That's odd.\n", cat->name);
  }
  return cat;
}

int main(int argc, char *argv[]) {
  id Object = oo_init();

  id Person = class_new(Object, struct Person, "Person");
  object_call("add_method", Person, "init", person_init);
  object_call("add_method", Person, "greet", person_greet);

  id Friend = class_new(Person, struct Person, "Friend");
  object_call("add_method", Friend, "greet", friend_greet);

  id Cat = class_new(Object, struct Cat, "Cat");
  object_call("add_method", Cat, "init", cat_init);
  object_call("add_method", Cat, "greet", cat_greet);

  id Array = object_call("find", Object, "Array");
  id array = object_new(Array, 1);

  id carl = object_call("push", array, object_new(Person, "Carl"));
  id jenny = object_call("push", array, object_new(Friend, "Jenny"));
  id sassy = object_call("push", array, object_new(Cat, "Sassy", 4, 0));
  id tp = object_call("push", array, object_new(Cat, "Thunder Pickles", 4, 1));

  for(int ii = 0; ii < 10; ii++) {
    object_call("push", array, tp);
  }

  // these calls are polymorphic because the behavior of the "greet"
  // method depends on the class of the object that it is being
  // invoked on.
  object_call("foreach", array, "greet");

  object_call("dump", jenny, stderr);
  object_call("dump", Array, stderr);
  //object_call("undefined", sassy);

  object_call("release", carl);
  object_call("release", jenny);
  object_call("release", sassy);
  object_call("release", tp);
  object_call("release", array);

  return 0;
}
