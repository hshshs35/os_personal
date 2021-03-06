/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "list.h"


#define METADATA_SIZE sizeof(struct list_elem)
#define _XOPEN_SOURCE_EXTENDED 1

struct list blk_list;
int init = 0;

void *mm_malloc(size_t size) {
    /* YOUR CODE HERE */
    if (!init) {
        list_init(&blk_list);
        init = 1;
    }

    struct list_elem *e;
    size_t allocate_size = size + METADATA_SIZE;

    for (e = blk_list.head.next; e != &blk_list.tail; e = e->next) {
        if ((!e->free) || e->size < size)
            continue;

        size_t leftover = e->size - size - METADATA_SIZE;
        if (leftover > 0) {
            struct list_elem *blk_meta = (void *)(e) + METADATA_SIZE + e->size;
            blk_meta->size = leftover;
            blk_meta->free = 1;
            memset(blk_meta+METADATA_SIZE, 0, leftover);
            list_insert(e, blk_meta);
        }

        e->size = size;
        e->free = 0;
        memset(e+METADATA_SIZE, 0, e->size);

        return e + METADATA_SIZE;
    }

    struct list_elem *header = sbrk(allocate_size);

    if (header == (void *)-1)
        return NULL;

    memset(header, 0, allocate_size);
    header->size = size;
    header->free = 0;
    list_push_back(&blk_list, header);

    void *address = (void *)header+METADATA_SIZE;

    return address;
}

void *mm_realloc(void *ptr, size_t size) {
    /* YOUR CODE HERE */
    struct list_elem *meta = ptr - METADATA_SIZE;
    mm_free(ptr);
    void *res = mm_malloc(size);
    void *extend = res + meta->size;
    memcpy(res, ptr, meta->size);
    memset(extend, 0, size - meta->size);
    return res;
}

void mm_free(void *ptr) {
    /* YOUR CODE HERE */
    struct list_elem *e = ptr-METADATA_SIZE;

    void *start  = e;
    void *end = ptr + e->size;

    if (e->prev != &blk_list.head && e->prev->free == 1) {
        start = (void *)e->prev - METADATA_SIZE;
        list_remove(e->prev);
    }
    if (e->next != &blk_list.tail && e->next->free == 1) {
        end = (void *)e->next + e->next->size;
        list_remove(e->next);
    }

    list_remove(e);

    int size = end - start - METADATA_SIZE;

    struct list_elem *blk = (struct list_elem *)start;
    blk->size = size;
    blk->free = 1;

    list_insert(e->prev, blk);
}