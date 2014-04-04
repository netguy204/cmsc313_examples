#ifndef POLY_H
#define POLY_H

typedef void* id;

id object_call(const char* method_name, id object, ...);
id object_supercall(id prev_method, id object, ...);

#define class_new(p, t, n) object_call("subclass", p, n, sizeof(t));
#define object_new(c, ...) object_call("init", object_call("alloc", c), ##__VA_ARGS__)

id oo_init();

#endif
