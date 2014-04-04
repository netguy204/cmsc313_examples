#include "poly.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>

struct Class_;

typedef struct {
  struct Class_* class;
  int refcount;
} Header;

struct Method;

typedef id (*IMP)(struct Method*, id obj, ...);
#define header_get(obj) ((Header*)((char*)obj - sizeof(Header)))

id header_get_(id obj) {
  return header_get(obj);
}

typedef struct Method_ {
  struct Class_* owner;
  IMP imp;
  char name[32];
  struct Method_* next;
} Method;

typedef struct Class_ {
  struct Class_* parent;
  struct Class_* next;
  id method_class;
  Method* methods;
  size_t inst_size;
  char name[32];
} Class;

typedef struct {
  Class _;
  Class* classes;
} RootClass;

id object_malloc(size_t base_size, Class* class) {
  Header* header = malloc(base_size + sizeof(Header));
  header->class = class;
  header->refcount = 1;

  return (char*)header + sizeof(Header);
}

void object_free(id obj) {
  char* mem = (char*)obj;
  mem = mem - sizeof(Header);
  free(mem);
}

Method* method_add(Class* owner, Class* mclass, const char* name, IMP imp) {
  Method* method = object_malloc(sizeof(Method), mclass);
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

id error_call(Method* method, id obj, ...) {
  fprintf(stderr, "Method `%s' does not exist on class %s\n", method->name, header_get(obj)->class->name);
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
  return object_malloc(class->inst_size, class);
}

id class_add_method(Method* m, Class* class, const char* name, IMP imp) {
  Method* method = object_call("init", object_call("alloc", class->method_class), class, name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id class_root(Method* method, Class* class) {
  Class* root = class;
  while(root->parent) {
    root = root->parent;
  }
  return root;
}

id class_init(Method* method, Class* class, Class* parent, const char* name, size_t size) {
  // special case since this is used during bootstrap
  if(method) object_supercall(method, class);

  class->parent = parent;
  class->method_class = parent->method_class;
  class->methods = NULL;
  class->inst_size = size;
  strncpy(class->name, name, sizeof(class->name));

  // add our new class to the db
  RootClass* root = class_root(NULL, class);
  class->next = root->classes;
  root->classes = class;

  return class;
}

id class_subclass(Method* method, Class* class, const char* name, size_t size) {
  return object_call("init", object_call("alloc", header_get(class)->class), class, name, size);
}

id class_parent(Method* method, Class* class) {
  return class->parent;
}

id class_find(Method* method, Class* class, const char* name) {
  RootClass* root = object_call("root", class);
  Class* next = root->classes;
  while(next) {
    if(strcmp(next->name, name) == 0) {
      return next;
    }
    next = next->next;
  }
  return NULL;
}

id object_init(Method* method, id object) {
  return object;
}

id object_retain(Method* m, id object) {
  header_get(object)->refcount += 1;
  return object;
}

id object_release(Method* m, id object) {
  header_get(object)->refcount -= 1;
  if(header_get(object)->refcount <= 0) {
    object_call("finalize", object);
    object_free(object);
    return NULL;
  }
  return object;
}

id object_class(Method* m, id object) {
  return header_get(object)->class;
}

id object_finalize(Method* method, id obj, ...) {
  fprintf(stderr, "Finalizing object of class `%s'\n", header_get(obj)->class->name);
  return obj;
}

id object_dump(Method* m, id obj, FILE* target) {
  Class* class = object_call("class", obj);
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

// standard library stuff
struct Array {
  size_t size;
  size_t capacity;
  id* buffer;
};

id array_init(id method, struct Array* arr) {
  object_supercall(method, arr);

  arr->size = 0;
  arr->capacity = 10;
  arr->buffer = malloc(sizeof(id) * arr->capacity);
  return arr;
}

id array_finalize(id method, struct Array* arr) {
  object_supercall(method, arr);

  for(size_t i = 0; i < arr->size; ++i) {
    object_call("release", arr->buffer[i]);
  }

  free(arr->buffer);
  return arr;
}

id array_push(id method, struct Array* arr, id value) {
  if(arr->size == arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->buffer = realloc(arr->buffer, sizeof(id) * arr->capacity);
  }

  arr->buffer[arr->size++] = object_call("retain", value);
  return value;
}

id array_element_at(id method, struct Array* arr, size_t idx) {
  return arr->buffer[idx];
}

id array_foreach(id method, struct Array* arr, void(*fn)(id)) {
  for(size_t i = 0; i < arr->size; ++i) {
    fn(arr->buffer[i]);
  }
  return arr;
}

void oo_init(id* _object) {
  // initialize the "braid" of objects/classes/methods
  // Classes are Objects
  // The object, class, and method objects are all instances of a Class
  Class* class = object_malloc(sizeof(Class), NULL);
  RootClass* object = object_malloc(sizeof(RootClass), class);
  Class* method = object_malloc(sizeof(Class), class);
  header_get(class)->class = class;

  // Object is the root and holds global data for the system
  object->_.parent = NULL;
  object->_.method_class = method;
  object->_.methods = NULL;
  object->_.inst_size = 0;
  object->_.next = NULL;
  object->classes = (Class*)object;
  strcpy(object->_.name, "Object");

  class_init(NULL, class, (Class*)object, "Class", sizeof(Class));
  class_init(NULL, method, (Class*)object, "Method", sizeof(Method));

  // wire up the alloc method on class
  method_add(class, method, "alloc", (IMP)class_alloc_object);

  // and the init method on method/object
  method_add(method, method, "init", (IMP)method_init);
  method_add((Class*)object, method, "init", (IMP)object_init);

  // finally add the add_method method to class
  method_add(class, method, "add_method", (IMP)class_add_method);

  // and use this mechanism to add the init/finalize method to object
  object_call("add_method", object, "retain", object_retain);
  object_call("add_method", object, "release", object_release);
  object_call("add_method", object, "finalize", object_finalize);
  object_call("add_method", object, "dump", object_dump);
  object_call("add_method", object, "class", object_class);

  // and the init and subclass methods to class
  object_call("add_method", class, "init", class_init);
  object_call("add_method", class, "subclass", class_subclass);
  object_call("add_method", class, "parent", class_parent);
  object_call("add_method", class, "root", class_root);
  object_call("add_method", class, "find", class_find);

  id Array = class_new(object, struct Array, "Array");
  object_call("add_method", Array, "init", array_init);
  object_call("add_method", Array, "finalize", array_finalize);
  object_call("add_method", Array, "push", array_push);
  object_call("add_method", Array, "element_at", array_element_at);
  object_call("add_method", Array, "foreach", array_foreach);

  *_object = object;
}
