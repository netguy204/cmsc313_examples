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
  superinvoke(method, obj);

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
  superinvoke(method, cat);

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
  fprintf(stderr, "Finalizing object %s\n", invoke("crepr", obj));
  return obj;
}

int main(int argc, char *argv[]) {
  id Object = oo_init();

  // overwrite finalize so we can see when an object is finalized
  invoke("add_method", Object, "finalize", object_noisy_finalize);

  id Person = invoke("subclass", Object, "Person", sizeof(struct Person));
  invoke("add_method", Person, "init", person_init);
  invoke("add_method", Person, "greet", person_greet);

  id Friend = invoke("subclass", Person, "Friend", sizeof(struct Person));
  invoke("add_method", Friend, "greet", friend_greet);

  id Cat = invoke("subclass", Object, "Cat", sizeof(struct Cat));
  invoke("add_method", Cat, "init", cat_init);
  invoke("add_method", Cat, "greet", cat_greet);

  id Array = invoke("find", Object, "Array");
  id array = invoke("new", Array);

  id carl = invoke("autorelease", invoke("new", Person, "Carl"));
  id jenny = invoke("autorelease", invoke("new", Friend, "Jenny"));
  id sassy = invoke("autorelease", invoke("new", Cat, "Sassy", 4, 0));
  id tp = invoke("autorelease", invoke("new", Cat, "Thunder Pickles", 4, 1));

  invoke("push", array, carl);
  invoke("push", array, jenny);
  invoke("push", array, sassy);
  invoke("push", array, tp);

  for(int ii = 0; ii < 10; ii++) {
    invoke("push", array, tp);
  }

  // these invokes are polymorphic because the behavior of the "greet"
  // method depends on the class of the object that it is being
  // invoked on.
  invoke("foreach", array, "println", stdout);
  invoke("foreach", array, "greet");
  invoke("dump", Array, stdout);

  //invoke("undefined", sassy);
  invoke("release", array);
  invoke("release_pending", Object);

  return 0;
}
