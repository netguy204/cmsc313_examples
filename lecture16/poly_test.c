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
  supercall(method, obj);

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
  supercall(method, cat);

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

id object_noisy_finalize(id method, id obj) {
  fprintf(stderr, "Finalizing object %s\n", call("crepr", obj));
  return obj;
}

int main(int argc, char *argv[]) {
  id Object = oo_init();

  // overwrite finalize so we can see when an object is finalized
  call("add_method", Object, "finalize", object_noisy_finalize);

  id Person = class_new(Object, struct Person, "Person");
  call("add_method", Person, "init", person_init);
  call("add_method", Person, "greet", person_greet);

  id Friend = class_new(Person, struct Person, "Friend");
  call("add_method", Friend, "greet", friend_greet);

  id Cat = class_new(Object, struct Cat, "Cat");
  call("add_method", Cat, "init", cat_init);
  call("add_method", Cat, "greet", cat_greet);

  id Array = call("find", Object, "Array");
  id array = call("new", Array);

  id carl = call("autorelease", call("new", Person, "Carl"));
  id jenny = call("autorelease", call("new", Friend, "Jenny"));
  id sassy = call("autorelease", call("new", Cat, "Sassy", 4, 0));
  id tp = call("autorelease", call("new", Cat, "Thunder Pickles", 4, 1));

  call("push", array, carl);
  call("push", array, jenny);
  call("push", array, sassy);
  call("push", array, tp);

  for(int ii = 0; ii < 10; ii++) {
    call("push", array, tp);
  }

  // these calls are polymorphic because the behavior of the "greet"
  // method depends on the class of the object that it is being
  // invoked on.
  call("foreach", array, "println", stdout);
  call("foreach", array, "greet");
  call("dump", Array, stdout);

  //call("undefined", sassy);
  call("release", array);
  call("release_pending", Object);

  return 0;
}
