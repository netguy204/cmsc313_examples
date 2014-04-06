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
  size_t size;
  size_t refcount;
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
  size_t size = base_size + sizeof(struct Header);
  struct Header* header = malloc(size);
  header->class = class;
  header->size = size;
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

id class_find_method(id m, struct Class* class, const char* name) {
  struct Class* next_class = class;
  while(next_class) {
    struct Method* next_method = next_class->methods;
    while(next_method) {
      if(strcmp(next_method->name, name) == 0) {
        return next_method;
      }
      next_method = next_method->next;
    }
    next_class = next_class->parent;
  }

  return invoke("method_missing", class, name);
}

id method_init(id m, struct Method* method, struct Class* owner, const char* name, IMP imp) {
  invoke("superinvoke", m, method);
  method->owner = owner;
  method->imp = imp;
  strncpy(method->name, name, sizeof(method->name));
  method->next = NULL;
  return method;
}

id method_supermethod(id m, struct Method* method) {
  struct Class* class = method->owner->parent;
  return class_find_method(NULL, class, method->name);
}

id method_name(id m, struct Method* method) {
  return method->name;
}

id method_imp(id m, struct Method* method) {
  return method->imp;
}

// note, calls to method_invoke can include no more than 5 arguments
// because we're not using the tailcall trick here.
id method_invoke(id m, id method, id obj, id _1, id _2, id _3, id _4, id _5) {
  IMP fn = invoke("imp", method);
  return fn(method, obj, _1, _2, _3, _4, _5);
}

// note, calls to method_superinvoke can include no more than 5 arguments
// because we're not using the tailcall trick here.
id method_superinvoke(id m, id method, id obj, id _1, id _2, id _3, id _4, id _5) {
  return invoke("invoke", invoke("supermethod", method), obj, _1, _2, _3, _4, _5);
}

extern id class_new_object(id method, id obj, ...);

id class_alloc(id method, struct Class* class) {
  return object_malloc(class->inst_size, class);
}

id class_alloc_size(id method, id class, size_t size) {
  return object_malloc(size, class);
}

id class_add_method(id m, struct Class* class, const char* name, IMP imp) {
  struct Method* method = invoke("init", invoke("alloc", class->method_class), class, name, imp);
  method->next = class->methods;
  class->methods = method;
  return method;
}

id class_add_class_method(id m, struct Class* class, const char* name, IMP imp) {
  // use the metaclass as our target
  return invoke("add_method", invoke("class", class), name, imp);
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
  if(method) invoke("superinvoke", method, class);

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
  struct Class* new_metaclass = invoke("alloc", old_metaclass);
  char mcname[128];

  snprintf(mcname, sizeof(mcname), "%sMC", name); // name it
  new_metaclass = invoke("init", new_metaclass, old_metaclass, name, old_metaclass->inst_size);

  // now produce the class
  return invoke("init", invoke("alloc", new_metaclass), class, name, size);
}

id class_parent(id method, struct Class* class) {
  return class->parent;
}

id class_find_class(id method, struct Class* class, const char* name) {
  struct RootClass* root = invoke("root", class);
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
    invoke("finalize", object);
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
    invoke("release", header_obj(next));
    next = next->ar_next;
  }
  return object;
}

id object_finalize(id method, id obj, ...) {
  return obj;
}

id object_repr(id method, id obj) {
  struct Class* class = invoke("class", obj);
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "<%s: %p %lu bytes>", class->name, obj, header_get(obj)->size);
  id string = invoke("new", invoke("find_class", class, "String"), buffer);
  return invoke("autorelease", string);
}

id object_crepr(id method, id obj) {
  return invoke("cstring", invoke("repr", obj));
}

id object_print(id method, id obj, FILE* target) {
  fprintf(target, "%s", invoke("cstring", obj));
  return obj;
}

id object_println(id method, id obj, FILE* target) {
  fprintf(target, "%s\n", invoke("cstring", obj));
  return obj;
}

id object_dump(id m, id obj, FILE* target) {
  struct Class* class = invoke("class", obj);
  fprintf(target, "%s\n", invoke("cstring", obj));

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

id object_method_missing(id m, struct Class* class, const char* name) {
  fprintf(stderr, "Method `%s' does not exist on class %s\n", name, class->name);
  exit(1);
  return NULL;
}

// standard library stuff
struct String {
  char cstr[0];
};

id string_new(id method, id class, const char* init) {
  size_t len = strlen(init);
  struct String* str = invoke("alloc_size", class, len+1);
  memcpy(str->cstr, init, len);
  str->cstr[len] = '\0';

  return str;
}

id string_finalize(id method, struct String* str) {
  invoke("superinvoke", method, str);
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
  invoke("superinvoke", method, arr);

  arr->size = 0;
  arr->capacity = 10;
  arr->buffer = malloc(sizeof(id) * arr->capacity);
  return arr;
}

id array_finalize(id method, struct Array* arr) {
  invoke("superinvoke", method, arr);

  for(size_t i = 0; i < arr->size; ++i) {
    invoke("release", arr->buffer[i]);
  }

  free(arr->buffer);
  return arr;
}

id array_push(id method, struct Array* arr, id value) {
  if(arr->size == arr->capacity) {
    arr->capacity = arr->capacity * 2;
    arr->buffer = realloc(arr->buffer, sizeof(id) * arr->capacity);
  }

  arr->buffer[arr->size++] = invoke("retain", value);
  return value;
}

id array_element_at(id method, struct Array* arr, size_t idx) {
  return arr->buffer[idx];
}

id array_foreach(id method, struct Array* arr, const char* message, id _1, id _2, id _3, id _4) {
  for(size_t i = 0; i < arr->size; ++i) {
    invoke(message, arr->buffer[i], _1, _2, _3, _4);
  }
  return arr;
}

struct BoundMethod {
  struct Method _;
  id method;
  id _1;
};

id boundmethod_invoke(id m, struct BoundMethod* method, id obj, id _1, id _2, id _3, id _4) {
  return invoke("invoke", method->method, obj, method->_1, _1, _2, _3, _4);
}

id boundmethod_invoke_(id m, id obj, id _1, id _2, id _3, id _4) {
  return boundmethod_invoke(m, m, obj, _1, _2, _3, _4);
}

id boundmethod_init(id m, struct BoundMethod* method, id unbound, id _1) {
  char name[32];
  snprintf(name, sizeof(name), "%s (bound)", invoke("name", unbound));
  invoke("superinvoke", m, method, NULL, name, boundmethod_invoke_);

  method->method = invoke("retain", unbound);
  method->_1 = invoke("retain", _1);
  return method;
}

id boundmethod_finalize(id m, struct BoundMethod* method) {
  invoke("superinvoke", m, method);

  invoke("release", method);
  invoke("release", method->_1);
  return method;
}

id oo_init() {
  // initialize the "braid" of objects/classes/methods

  // Classes are Objects. The Object, Class, and Method objects are
  // all instances of a Class. An Object is an instance of a Class but
  // its instance has enough storage set aside for a RootClass so that
  // we can stash important global things there (like the class list
  // and the autorelease pool).
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

  // and flesh out enough of Method so that superinvoke will work
  method_add(Method, Method, "supermethod", (IMP)method_supermethod);
  method_add(Method, Method, "imp", (IMP)method_imp);
  method_add(Method, Method, "invoke", (IMP)method_invoke);
  method_add(Method, Method, "superinvoke", (IMP)method_superinvoke);

  // finally add the add_method method to class
  method_add(Class, Method, "add_method", (IMP)class_add_method);

  // and some nice introspection methods on Method
  invoke("add_method", Method, "name", method_name);

  // and use this mechanism to add the init/finalize method to Object
  invoke("add_method", Object, "retain", object_retain);
  invoke("add_method", Object, "release", object_release);
  invoke("add_method", Object, "autorelease", object_autorelease);
  invoke("add_method", Object, "finalize", object_finalize);
  invoke("add_method", Object, "method_missing", object_method_missing);
  invoke("add_method", Object, "dump", object_dump);
  invoke("add_method", Object, "class", object_class);
  invoke("add_method", Object, "string", object_repr);
  invoke("add_method", Object, "cstring", object_crepr);
  invoke("add_method", Object, "repr", object_repr);
  invoke("add_method", Object, "crepr", object_crepr);
  invoke("add_method", Object, "print", object_print);
  invoke("add_method", Object, "println", object_println);
  invoke("add_method", Object, "release_pending", object_release_pending);

  // and the init and subclass methods to Class
  invoke("add_method", Class, "add_class_method", class_add_class_method);
  invoke("add_method", Class, "find_method", class_find_method);
  invoke("add_method", Class, "find_class", class_find_class);
  invoke("add_method", Class, "alloc_size", class_alloc_size);
  invoke("add_method", Class, "init", class_init);
  invoke("add_method", Class, "new", class_new_object);
  invoke("add_method", Class, "release", class_release);
  invoke("add_method", Class, "subclass", class_subclass);
  invoke("add_method", Class, "parent", class_parent);
  invoke("add_method", Class, "root", class_root);

  id String = invoke("subclass", Object, "String", sizeof(struct String));
  invoke("add_class_method", String, "new", string_new);
  invoke("add_method", String, "finalize", string_finalize);
  invoke("add_method", String, "cstring", string_cstring);
  invoke("add_method", String, "string", string_string);

  id Array = invoke("subclass", Object, "Array", sizeof(struct Array));
  invoke("add_method", Array, "init", array_init);
  invoke("add_method", Array, "finalize", array_finalize);
  invoke("add_method", Array, "push", array_push);
  invoke("add_method", Array, "element_at", array_element_at);
  invoke("add_method", Array, "foreach", array_foreach);

  id BoundMethod = invoke("subclass", Method, "BoundMethod", sizeof(struct BoundMethod));
  invoke("add_method", BoundMethod, "init", boundmethod_init);
  invoke("add_method", BoundMethod, "finalize", boundmethod_finalize);
  invoke("add_method", BoundMethod, "invoke", boundmethod_invoke);

  return Object;
}
