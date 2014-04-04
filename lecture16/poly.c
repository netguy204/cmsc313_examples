#include "poly.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Class;
struct Header;

struct Header {
  struct Class* class;
  struct Header* ar_next;
  int refcount;
};

struct Method;

typedef id (*IMP)(struct Method*, id obj, ...);
#define header_get(obj) ((struct Header*)((char*)obj - sizeof(struct Header)))
#define header_obj(hdr) ((id)((char*)hdr + sizeof(struct Header)))

id header_get_(id obj) {
  return header_get(obj);
}

struct Method {
  struct Class* owner;
  IMP imp;
  char name[32];
  struct Method* next;
};

struct Class {
  struct Class* parent;
  struct Class* next;
  id method_class;
  struct Method* methods;
  size_t inst_size;
  char name[32];
};

struct RootClass {
  struct Class _;
  struct Class* classes;
  struct Header* ar_queue;
};

id object_malloc(size_t base_size, struct Class* class) {
  struct Header* header = malloc(base_size + sizeof(struct Header));
  header->class = class;
  header->refcount = 1;
  header->ar_next = NULL;

  return (char*)header + sizeof(struct Header);
}

void object_free(id obj) {
  char* mem = (char*)obj;
  mem = mem - sizeof(struct Header);
  free(mem);
}

struct Method* method_add(struct Class* owner, struct Class* mclass, const char* name, IMP imp) {
  struct Method* method = object_malloc(sizeof(struct Method), mclass);
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = owner ? owner->methods : NULL;
  if(owner) owner->methods = method;
  return method;
}

IMP method_imp(struct Method* method) {
  return method->imp;
}

id error_call(struct Method* method, id obj, ...) {
  fprintf(stderr, "Method `%s' does not exist on class %s\n", method->name, header_get(obj)->class->name);
  exit(1);
  return NULL;
}

id class_find_method(struct Method* m, struct Class* class, const char* name) {
  if(class == NULL) return method_add(NULL, NULL, name, (IMP)error_call);

  struct Method* method = class->methods;
  while(method) {
    if(strcmp(method->name, name) == 0) {
      return method;
    }
    method = method->next;
  }
  return class_find_method(m, class->parent, name);
}

id method_find_supermethod(struct Method* m, struct Method* method) {
  struct Class* class = method->owner->parent;
  return class_find_method(NULL, class, method->name);
}

id method_init(struct Method* m, struct Method* method, struct Class* owner, const char* name, IMP imp) {
  object_supercall(m, method);
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = NULL;
  return method;
}

id class_alloc_object(struct Method* method, struct Class* class) {
  return object_malloc(class->inst_size, class);
}

id class_add_method(struct Method* m, struct Class* class, const char* name, IMP imp) {
  struct Method* method = object_call("init", object_call("alloc", class->method_class), class, name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id class_root(struct Method* method, struct Class* class) {
  struct Class* root = class;
  while(root->parent) {
    root = root->parent;
  }
  return root;
}

id class_init(struct Method* method, struct Class* class, struct Class* parent,
              const char* name, size_t size) {
  // special case since this is used during bootstrap
  if(method) object_supercall(method, class);

  class->parent = parent;
  class->method_class = parent->method_class;
  class->methods = NULL;
  class->inst_size = size;
  strncpy(class->name, name, sizeof(class->name));

  // add our new class to the db
  struct RootClass* root = class_root(NULL, class);
  class->next = root->classes;
  root->classes = class;

  return class;
}

id class_subclass(struct Method* method, struct Class* class, const char* name, size_t size) {
  return object_call("init", object_call("alloc", header_get(class)->class), class, name, size);
}

id class_parent(struct Method* method, struct Class* class) {
  return class->parent;
}

id class_find(struct Method* method, struct Class* class, const char* name) {
  struct RootClass* root = object_call("root", class);
  struct Class* next = root->classes;
  while(next) {
    if(strcmp(next->name, name) == 0) {
      return next;
    }
    next = next->next;
  }
  return NULL;
}

id class_release(struct Method* method, id class) {
  return class;
}

id object_init(struct Method* method, id object) {
  return object;
}

id object_retain(struct Method* m, id object) {
  header_get(object)->refcount += 1;
  return object;
}

id object_release(struct Method* m, id object) {
  header_get(object)->refcount -= 1;
  if(header_get(object)->refcount <= 0) {
    object_call("finalize", object);
    object_free(object);
    return NULL;
  }
  return object;
}

id object_class(struct Method* m, id object) {
  return header_get(object)->class;
}

id object_autorelease(struct Method* m, id object) {
  struct RootClass* root = class_root(NULL, object_class(NULL, object));
  struct Header* header = header_get(object);
  header->ar_next = root->ar_queue;
  root->ar_queue = header;
  return object;
}

id object_release_pending(struct Method* m, id object) {
  struct RootClass* root = class_root(NULL, object_class(NULL, object));
  struct Header* next = root->ar_queue;
  while(next) {
    object_call("release", header_obj(next));
    next = next->ar_next;
  }
  root->ar_queue = NULL;
  return object;
}

id object_finalize(struct Method* method, id obj, ...) {
  fprintf(stderr, "Finalizing object of class `%s'\n", header_get(obj)->class->name);
  return obj;
}

id object_string(struct Method* method, id obj) {
  struct Class* class = object_call("class", obj);
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "<%s: %p %lu bytes>", class->name, obj, class->inst_size);
  return object_call("autorelease", object_new(object_call("find", class, "String"), buffer));
}

id object_cstring(struct Method* method, id obj) {
  return object_call("cstring", object_call("string", obj));
}

id object_print(struct Method* method, id obj, FILE* target) {
  fprintf(target, "%s", object_call("cstring", obj));
  return obj;
}

id object_println(struct Method* method, id obj, FILE* target) {
  fprintf(target, "%s\n", object_call("cstring", obj));
  return obj;
}

id object_dump(struct Method* m, id obj, FILE* target) {
  struct Class* class = object_call("class", obj);
  fprintf(target, "%s\n", object_call("cstring", obj));

  while(class) {
    fprintf(target, "%s\n", class->name);
    struct Method* method = class->methods;
    while(method) {
      fprintf(target, "- %s\n", method->name);
      method = method->next;
    }
    class = class->parent;
  }
  return obj;
}

// standard library stuff
struct String {
  char* cstr;
};

id string_init(id method, struct String* str, const char* init) {
  size_t len = strlen(init);
  str->cstr = malloc(len + 1);
  memcpy(str->cstr, init, len);
  str->cstr[len] = '\0';

  return str;
}

id string_finalize(id method, struct String* str) {
  object_supercall(method, str);
  free(str->cstr);
  return str;
}

id string_cstring(id method, struct String* str) {
  return str->cstr;
}

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

id array_foreach(id method, struct Array* arr, const char* message, id _1, id _2, id _3, id _4) {
  for(size_t i = 0; i < arr->size; ++i) {
    object_call(message, arr->buffer[i], _1, _2, _3, _4);
  }
  return arr;
}

id oo_init() {
  // initialize the "braid" of objects/classes/methods
  // Classes are Objects
  // The object, class, and method objects are all instances of a Class
  struct Class* Class = object_malloc(sizeof(struct Class), NULL);
  struct RootClass* Object = object_malloc(sizeof(struct RootClass), Class);
  struct Class* Method = object_malloc(sizeof(struct Class), Class);
  header_get(Class)->class = Class;

  // Object is the root and holds global data for the system
  Object->_.parent = NULL;
  Object->_.method_class = Method;
  Object->_.methods = NULL;
  Object->_.inst_size = 0;
  Object->_.next = NULL;
  Object->classes = (struct Class*)Object;
  Object->ar_queue = NULL;

  strcpy(Object->_.name, "Object");

  class_init(NULL, Class, (struct Class*)Object, "Class", sizeof(struct Class));
  class_init(NULL, Method, (struct Class*)Object, "Method", sizeof(struct Method));

  // wire up the alloc method on class
  method_add(Class, Method, "alloc", (IMP)class_alloc_object);

  // and the init method on method/object
  method_add(Method, Method, "init", (IMP)method_init);
  method_add((struct Class*)Object, Method, "init", (IMP)object_init);

  // finally add the add_method method to class
  method_add(Class, Method, "add_method", (IMP)class_add_method);

  // and use this mechanism to add the init/finalize method to object
  object_call("add_method", Object, "retain", object_retain);
  object_call("add_method", Object, "release", object_release);
  object_call("add_method", Object, "autorelease", object_autorelease);
  object_call("add_method", Object, "finalize", object_finalize);
  object_call("add_method", Object, "dump", object_dump);
  object_call("add_method", Object, "class", object_class);
  object_call("add_method", Object, "string", object_string);
  object_call("add_method", Object, "cstring", object_cstring);
  object_call("add_method", Object, "print", object_print);
  object_call("add_method", Object, "println", object_println);
  object_call("add_method", Object, "release_pending", object_release_pending);

  // and the init and subclass methods to class
  object_call("add_method", Class, "init", class_init);
  object_call("add_method", Class, "release", class_release);
  object_call("add_method", Class, "subclass", class_subclass);
  object_call("add_method", Class, "parent", class_parent);
  object_call("add_method", Class, "root", class_root);
  object_call("add_method", Class, "find", class_find);

  id String = class_new(Object, struct String, "String");
  object_call("add_method", String, "init", string_init);
  object_call("add_method", String, "finalize", string_finalize);
  object_call("add_method", String, "cstring", string_cstring);

  id Array = class_new(Object, struct Array, "Array");
  object_call("add_method", Array, "init", array_init);
  object_call("add_method", Array, "finalize", array_finalize);
  object_call("add_method", Array, "push", array_push);
  object_call("add_method", Array, "element_at", array_element_at);
  object_call("add_method", Array, "foreach", array_foreach);

  return Object;
}
