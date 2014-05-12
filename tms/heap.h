#ifndef HEAP_H
#define HEAP_H

typedef struct Heap {
    int	size, allocated;
    struct helem_t **data;
} Heap;

typedef struct helem_t {
    int value;
    void *data;
} helem_t;

Heap *new_heap();
void delete_heap(Heap *h);
void heap_add(Heap *h, int value, void *data);
void *heap_remove(Heap *h);

#endif
