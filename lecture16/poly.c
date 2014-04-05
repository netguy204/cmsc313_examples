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

extern id class_new_object(id method, id obj, ...);

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

id class_find_method(id m, struct Class* class, const char* name) {
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

id method_find_supermethod(id m, struct Method* method) {
  struct Class* class = method->owner->parent;
  return class_find_method(NULL, class, method->name);
}

id method_init(id m, struct Method* method, struct Class* owner, const char* name, IMP imp) {
  supercall(m, method);
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = NULL;
  return method;
}

id class_alloc(id method, struct Class* class) {
  return object_malloc(class->inst_size, class);
}

id class_alloc_size(id method, id class, size_t size) {
  return object_malloc(size, class);
}

id class_add_method(id m, struct Class* class, const char* name, IMP imp) {
  struct Method* method = call("init", call("alloc", class->method_class), class, name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id class_add_class_method(id m, struct Class* class, const char* name, IMP imp) {
  // use the metaclass as our target
  return call("add_method", call("class", class), name, imp);
}

id class_root(id method, struct Class* class) {
  struct Class* root = class;
  while(root->parent) {
    root = root->parent;
  }
  return root;
}

id class_init(id method, struct Class* class, struct Class* parent, const char* name, size_t size) {
  // special case since this is used during bootstrap
  if(method) supercall(method, class);

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

id class_subclass(id method, id class, const char* name, size_t size) {
  // allocate the metaclass
  struct Class* old_metaclass = header_get(class)->class;
  struct Class* new_metaclass = call("alloc", old_metaclass);
  char mcname[128];

  snprintf(mcname, sizeof(mcname), "%sMC", name); // name it
  new_metaclass = call("init", new_metaclass, old_metaclass, name, old_metaclass->inst_size);

  // now produce the class
  return call("init", call("alloc", new_metaclass), class, name, size);
}

id class_parent(id method, struct Class* class) {
  return class->parent;
}

id class_find(id method, struct Class* class, const char* name) {
  struct RootClass* root = call("root", class);
  struct Class* next = root->classes;
  while(next) {
    if(strcmp(next->name, name) == 0) {
      return next;
    }
    next = next->next;
  }
  return NULL;
}

id class_release(id method, id class) {
  return class;
}

id object_init(id method, id object) {
  return object;
}

id object_retain(id m, id object) {
  header_get(object)->refcount += 1;
  return object;
}

id object_release(id m, id object) {
  header_get(object)->refcount -= 1;
  if(header_get(object)->refcount <= 0) {
    call("finalize", object);
    object_free(object);
    return NULL;
  }
  return object;
}

id object_class(id m, id object) {
  return header_get(object)->class;
}

id object_autorelease(id m, id object) {
  struct RootClass* root = class_root(NULL, object_class(NULL, object));
  struct Header* header = header_get(object);
  header->ar_next = root->ar_queue;
  root->ar_queue = header;
  return object;
}

id object_release_pending(id m, id object) {
  struct RootClass* root = class_root(NULL, object_class(NULL, object));
  struct Header* next = root->ar_queue;
  root->ar_queue = NULL;

  while(next) {
    call("release", header_obj(next));
    next = next->ar_next;
  }
  return object;
}

id object_finalize(id method, id obj, ...) {
  return obj;
}

id object_repr(id method, id obj) {
  struct Class* class = call("class", obj);
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "<%s: %p %lu bytes>", class->name, obj, class->inst_size);
  id string = call("new", call("find", class, "String"), buffer);
  return call("autorelease", string);
}

id object_crepr(id method, id obj) {
  return call("cstring", call("repr", obj));
}

id object_print(id method, id obj, FILE* target) {
  fprintf(target, "%s", call("cstring", obj));
  return obj;
}

id object_println(id method, id obj, FILE* target) {
  fprintf(target, "%s\n", call("cstring", obj));
  return obj;
}

id object_dump(id m, id obj, FILE* target) {
  struct Class* class = call("class", obj);
  fprintf(target, "%s\n", call("cstring", obj));

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
  char cstr[0];
};

id string_new(id method, id class, const char* init) {
  size_t len = strlen(init);
  struct String* str = call("alloc_size", class, len+1);
  memcpy(str->cstr, init, len);
  str->cstr[len] = '\0';

  return str;
}

id string_finalize(id method, struct String* str) {
  supercall(method, str);
  return str;
}

id string_cstring(id method, struct String* str) {
  return str->cstr;
}

id string_string(id method, id string) {
  return string;
}

struct Array {
  size_t size;
  size_t capacity;
  id* buffer;
};

id array_init(id method, struct Array* arr) {
  supercall(method, arr);

  arr->size = 0;
  arr->capacity = 10;
  arr->buffer = malloc(sizeof(id) * arr->capacity);
  return arr;
}

id array_finalize(id method, struct Array* arr) {
  supercall(method, arr);

  for(size_t i = 0; i < arr->size; ++i) {
    call("release", arr->buffer[i]);
  }

  free(arr->buffer);
  return arr;
}

id array_push(id method, struct Array* arr, id value) {
  if(arr->size == arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->buffer = realloc(arr->buffer, sizeof(id) * arr->capacity);
  }

  arr->buffer[arr->size++] = call("retain", value);
  return value;
}

id array_element_at(id method, struct Array* arr, size_t idx) {
  return arr->buffer[idx];
}

id array_foreach(id method, struct Array* arr, const char* message, id _1, id _2, id _3, id _4) {
  for(size_t i = 0; i < arr->size; ++i) {
    call(message, arr->buffer[i], _1, _2, _3, _4);
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
  method_add(Class, Method, "alloc", (IMP)class_alloc);

  // and the init method on method/object
  method_add(Method, Method, "init", (IMP)method_init);
  method_add((struct Class*)Object, Method, "init", (IMP)object_init);

  // finally add the add_method method to class
  method_add(Class, Method, "add_method", (IMP)class_add_method);

  // and use this mechanism to add the init/finalize method to object
  call("add_method", Object, "retain", object_retain);
  call("add_method", Object, "release", object_release);
  call("add_method", Object, "autorelease", object_autorelease);
  call("add_method", Object, "finalize", object_finalize);
  call("add_method", Object, "dump", object_dump);
  call("add_method", Object, "class", object_class);
  call("add_method", Object, "string", object_repr);
  call("add_method", Object, "cstring", object_crepr);
  call("add_method", Object, "repr", object_repr);
  call("add_method", Object, "crepr", object_crepr);
  call("add_method", Object, "print", object_print);
  call("add_method", Object, "println", object_println);
  call("add_method", Object, "release_pending", object_release_pending);

  // and the init and subclass methods to class
  call("add_method", Class, "add_class_method", class_add_class_method);
  call("add_method", Class, "alloc_size", class_alloc_size);
  call("add_method", Class, "init", class_init);
  call("add_method", Class, "new", class_new_object);
  call("add_method", Class, "release", class_release);
  call("add_method", Class, "subclass", class_subclass);
  call("add_method", Class, "parent", class_parent);
  call("add_method", Class, "root", class_root);
  call("add_method", Class, "find", class_find);

  id String = class_new(Object, struct String, "String");
  call("add_class_method", String, "new", string_new);
  call("add_method", String, "finalize", string_finalize);
  call("add_method", String, "cstring", string_cstring);
  call("add_method", String, "string", string_string);

  id Array = class_new(Object, struct Array, "Array");
  call("add_method", Array, "init", array_init);
  call("add_method", Array, "finalize", array_finalize);
  call("add_method", Array, "push", array_push);
  call("add_method", Array, "element_at", array_element_at);
  call("add_method", Array, "foreach", array_foreach);

  return Object;
}
