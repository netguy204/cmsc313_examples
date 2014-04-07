#ifndef POLY_H
#define POLY_H

typedef void* id;

id invoke(const char* method_name, id object, ...);
id oo_init();

#endif
