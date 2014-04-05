#ifndef POLY_H
#define POLY_H

typedef void* id;

id call(const char* method_name, id object, ...);
id supercall(id prev_method, id object, ...);

#define class_new(p, t, n) call("subclass", p, n, sizeof(t));

id oo_init();

#endif
