#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

typedef struct {
    void* vec;
    size_t ce;
    size_t cap;
    size_t tamElem;
} Vector;

typedef struct {
    long dni;
    int indice;
} Indice;

#endif // FUNCIONES_H_INCLUDED
