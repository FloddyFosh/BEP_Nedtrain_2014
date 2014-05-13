#include "alles.h"

#include "heap.h"

#define heap_parent(i)      ((i-1) / 2)
#define heap_right(i)       (2*i + 2)
#define heap_left(i)        (2*i + 1)

Heap *new_heap() {
    Heap *h = (Heap *) malloc(sizeof(Heap));

    h->size = 0;
    h->data = (helem_t **) malloc(3 * sizeof(helem_t *));
    h->allocated = 3;
    return h;
}

void delete_heap(Heap *h) {
    free(h->data);
    free(h);
}

void heap_grow(Heap *h) {
    h->data = (helem_t **) realloc(h->data, 2 * (h->allocated + 1) * sizeof(helem_t *));
    h->allocated = 2 * (h->allocated + 1);
}

void heap_sift_up(Heap *h, int i) {
    int parent;
    helem_t *tmp;

    if(i != 0) {
        parent = heap_parent(i);
        if(h->data[parent]->value > h->data[i]->value) {
            tmp = h->data[parent];
            h->data[parent] = h->data[i];
            h->data[i] = tmp;
            heap_sift_up(h, parent);
        }
    }
}

void heap_sift_down(Heap *h, int i) {
    int l, r, min;
    helem_t *tmp;

    l = heap_left(i);
    r = heap_right(i);

    if(r > h->size) {
        if(l > h->size) return;
        else min = l;
    } else {
        if(h->data[l]->value <= h->data[r]->value) min = l;
        else min = r;
    }
    if(h->data[i]->value > h->data[min]->value) {
        tmp = h->data[min];
        h->data[min] = h->data[i];
        h->data[i] = tmp;
        heap_sift_down(h, min);
    }
}

void heap_add(Heap *h, int value, void *data) {
    if(h->size+1 == h->allocated) {
        heap_grow(h);
    }

    h->data[h->size] = new helem_t;
    h->data[h->size]->value = value;
    h->data[h->size]->data = data;
    heap_sift_up(h, h->size);
    h->size++;
}

void *heap_remove(Heap *h) {
    void *ret;

    if(h->size == 0) return NULL;
    ret = h->data[0]->data;
    h->data[0] = h->data[h->size - 1];
    h->size--;
    if(h->size > 0) heap_sift_down(h, 0);
    return ret;
}

