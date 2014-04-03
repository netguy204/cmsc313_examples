#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

struct Object;
struct Method;
typedef void* id;

typedef id (*IMP)(struct Method*, struct Object* obj, ...);
id object_call(const char* method_name, id object, ...);

typedef struct Method_ {
  IMP imp;
  char name[32];
  struct Method_* next;
} Method;

Method* method_new(const char* name, IMP imp) {
  Method* method = (Method*)malloc(sizeof(Method));
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = NULL;
  return method;
}

typedef struct Class_ {
  struct Class_ *parent;
  Method* methods;
  size_t inst_size;
} Class;

typedef struct Object {
  Class* class;
} Object;

Class* class_new(Class* parent, size_t inst_size) {
  Class* class = (Class*)malloc(sizeof(Class));
  class->parent = parent;
  class->methods = NULL;
  class->inst_size = inst_size;
  return class;
}

Method* class_find_method(Class* class, const char* name) {
  if(class == NULL) return NULL;

  Method* method = class->methods;
  while(method) {
    if(strcmp(method->name, name) == 0) {
      return method;
    }
    method = method->next;
  }
  return class_find_method(class->parent, name);
}

Method* class_add_method(Class* class, const char* name, IMP imp) {
  Method* method = method_new(name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id object_new(Class* class) {
  Object* object = (Object*)malloc(class->inst_size);
  object->class = class;
  return object;
}

void object_free(id object) {
  free(object);
}

/*
 * Define the layout of the memory associated with a Person
 * instance. We'll create the classes later.
 */
typedef struct {
  Object _;
  char name[32];
} Person;

id person_init(Method* method, Person* obj, const char* name) {
  strncpy(obj->name, name, sizeof(obj->name));
  return obj;
}

id person_greet(Method* method, Person* obj, const char* greeter) {
  printf("Bob says, \"Hello, %s\".\n", obj->name, greeter);
  return obj;
}

id friend_greet(Method* method, Person* obj, const char* greeter) {
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

id cat_init(Method* method, Cat* cat, const char* name, int legs, int alive) {
  strncpy(cat->name, name, sizeof(cat->name));
  cat->legs = legs;
  cat->alive = alive;
  return cat;
}

id cat_greet(Method* method, Cat* cat, const char* greeter) {
  if(cat->alive) {
    printf("The %d legged cat named %s coldly ignores %s.\n", cat->legs, cat->name, greeter);
  } else {
    printf("%s is talking to a dead cat named %s. That's odd.\n", greeter, cat->name);
  }
  return cat;
}

int main(int argc, char *argv[]) {
  Class* CPerson = class_new(NULL, sizeof(Person));
  class_add_method(CPerson, "init", (IMP)person_init);
  class_add_method(CPerson, "greet", (IMP)person_greet);

  Class* CFriend = class_new(CPerson, sizeof(Person));
  class_add_method(CFriend, "greet", (IMP)friend_greet);

  Class* CCat = class_new(CCat, sizeof(CCat));
  class_add_method(CCat, "init", (IMP)cat_init);
  class_add_method(CCat, "greet", (IMP)cat_greet);

  id carl = object_call("init", object_new(CPerson), "Carl");
  id jenny = object_call("init", object_new(CFriend), "Jenny");
  id sassy = object_call("init", object_new(CCat), "Sassy", 4, 0);
  id tp = object_call("init", object_new(CCat), "Thunder Pickles", 4, 1);

  // these calls are polymorphic because the behavior of the "greet"
  // method depends on the class of the object that it is being
  // invoked on.
  object_call("greet", carl, "Bob");
  object_call("greet", jenny, "Bob");
  object_call("greet", sassy, "Bob");
  object_call("greet", tp, "Bob");

  return 0;
}
