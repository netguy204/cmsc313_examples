#include "poly.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Each instance of a Person class should have this structure
 * associated with it. This is where we store all of our instance
 * level data. We're effectively defining the memebers of the
 * instance.
 */
struct Person {
  char name[32];
};

id person_init(id method, struct Person* obj, const char* name) {
  invoke("superinvoke", method, obj);

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
 * Each instance of a Cat should have this data associated with it.
 */
struct Cat {
  char name[32];
  int legs;
  int alive;
};

id cat_init(id method, struct Cat* cat, const char* name, int legs, int alive) {
  invoke("superinvoke", method, cat);

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

id cat_dosomething(id m, struct Cat* cat, const char* name) {
  printf("%s doesn't know how to `%s'\n", cat->name, name);
  return cat;
}

id cat_method_missing(id m, id class, const char* name) {
  id BoundMethod = invoke("find_class", class, "BoundMethod");

  id String = invoke("find_class", class, "String");
  id name_string = invoke("autorelease", invoke("new", String, name));
  id target = invoke("find_method", class, "dosomething");
  return invoke("bind", target, name_string);
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
  invoke("add_method", Cat, "dosomething", cat_dosomething);
  invoke("add_class_method", Cat, "method_missing", cat_method_missing);

  id Array = invoke("find_class", Object, "Array");
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

  invoke("sing", sassy);
  invoke("dance", sassy);
  invoke("make_sushi", sassy);

  invoke("release", array);
  invoke("release_pending", Object);

  return 0;
}
