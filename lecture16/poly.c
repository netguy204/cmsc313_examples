#include "poly.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

struct Object;
struct Method;

typedef id (*IMP)(struct Method*, struct Object* obj, ...);

typedef struct Method_ {
  Object _;
  struct Class_* owner;
  IMP imp;
  char name[32];
  struct Method_* next;
} Method;

typedef struct Class_ {
  Object _;
  struct Class_* parent;
  id method_class;
  Method* methods;
  size_t inst_size;
  char name[32];
} Class;

Method* method_add(Class* owner, Class* mclass, const char* name, IMP imp) {
  Method* method = malloc(sizeof(Method));
  method->_.class = mclass;
  method->_.refcount = 1;
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = owner ? owner->methods : NULL;
  if(owner) owner->methods = method;
  return method;
}

IMP method_imp(Method* method) {
  return method->imp;
}

id error_call(Method* method, Object* obj, ...) {
  fprintf(stderr, "Method `%s' does not exist on class %s\n", method->name, obj->class->name);
  exit(1);
  return NULL;
}

id class_find_method(Method* m, Class* class, const char* name) {
  if(class == NULL) return method_add(NULL, NULL, name, (IMP)error_call);

  Method* method = class->methods;
  while(method) {
    if(strcmp(method->name, name) == 0) {
      return method;
    }
    method = method->next;
  }
  return class_find_method(m, class->parent, name);
}

id method_find_supermethod(Method* m, Method* method) {
  Class* class = method->owner->parent;
  return class_find_method(NULL, class, method->name);
}

id method_init(Method* m, Method* method, Class* owner, const char* name, IMP imp) {
  object_supercall(m, method);
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = NULL;
  return method;
}

id class_alloc_object(Method* method, Class* class) {
  Object* obj = malloc(class->inst_size);
  obj->class = class;
  obj->refcount = 1;
  return obj;
}

id class_add_method(Method* m, Class* class, const char* name, IMP imp) {
  Method* method = object_call("init", object_call("alloc", class->method_class), class, name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id class_init(Method* method, Class* class, Class* parent, const char* name, size_t size) {
  // special case since this is used during bootstrap
  if(method) object_supercall(method, class);

  class->parent = parent;
  class->method_class = parent->method_class;
  class->methods = NULL;
  class->inst_size = size;
  strncpy(class->name, name, sizeof(class->name));
  return class;
}

id class_subclass(Method* method, Class* class, const char* name, size_t size) {
  return object_call("init", object_call("alloc", class->_.class), class, name, size);
}

id object_init(Method* method, id object) {
  return object;
}

id object_retain(Method* m, Object* object) {
  object->refcount += 1;
  return object;
}

id object_release(Method* m, Object* object) {
  object->refcount -= 1;
  if(object->refcount <= 0) {
    object_call("finalize", object);
    free(object);
    return NULL;
  }
  return object;
}

id object_finalize(Method* method, Object* obj, ...) {
  fprintf(stderr, "Finalizing object of class `%s'\n", obj->class->name);
  return obj;
}

id object_dump(Method* m, Object* obj, FILE* target) {
  Class* class = obj->class;
  fprintf(target, "<%s: %p> (%lu bytes)\n", class->name, obj, class->inst_size);

  while(class) {
    fprintf(target, "%s\n", class->name);
    Method* method = class->methods;
    while(method) {
      fprintf(target, "- %s\n", method->name);
      method = method->next;
    }
    class = class->parent;
  }
  return obj;
}

void oo_init(id* _object) {
  // initialize the "braid" of objects/classes/methods
  Class* object = malloc(sizeof(Class));
  Class* class = malloc(sizeof(Class));
  Class* method = malloc(sizeof(Class));

  // Classes are Objects
  // The object, class, and method objects are all instances of a Class
  object->_.class = class;
  class->_.class = class;
  method->_.class = class;

  object->_.refcount = 1;
  class->_.refcount = 1;
  method->_.refcount = 1;

  // Object is the root (but things bottom out at NULL)
  object->parent = NULL;
  object->method_class = method;
  object->methods = NULL;
  object->inst_size = sizeof(Object);
  strcpy(object->name, "Object");

  class_init(NULL, class, object, "Class", sizeof(Class));
  class_init(NULL, method, object, "Method", sizeof(Method));

  // wire up the alloc method on class
  method_add(class, method, "alloc", (IMP)class_alloc_object);

  // and the init method on method/object
  method_add(method, method, "init", (IMP)method_init);
  method_add(object, method, "init", (IMP)object_init);

  // finally add the add_method method to class
  method_add(class, method, "add_method", (IMP)class_add_method);

  // and use this mechanism to add the init/finalize method to object
  object_call("add_method", object, "retain", object_retain);
  object_call("add_method", object, "release", object_release);
  object_call("add_method", object, "finalize", object_finalize);
  object_call("add_method", object, "dump", object_dump);

  // and the init and subclass methods to class
  object_call("add_method", class, "init", class_init);
  object_call("add_method", class, "subclass", class_subclass);

  *_object = object;
}
