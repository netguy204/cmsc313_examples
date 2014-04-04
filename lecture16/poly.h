#ifndef POLY_H
#define POLY_H

struct Class_;

typedef struct Object {
  struct Class_* class;
  int refcount;
} Object;

typedef void* id;

id object_call(const char* method_name, id object, ...);
id object_supercall(id prev_method, id object, ...);

#define class_new(p, t) object_call("subclass", p, #t, sizeof(t))
#define class_new2(p, t, n) object_call("subclass", p, n, sizeof(t));

#define object_new(c, ...) object_call("init", object_call("alloc", c), __VA_ARGS__)

void oo_init(id* _object);

#endif
