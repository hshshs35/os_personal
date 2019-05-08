#include "list.h"

void
list_init (struct list *list) {
    ASSERT (list != null)
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

void
list_elem_init (struct list_elem *elem, int size, int free) {
    elem->size = size;
    elem->free = free;
}

void
list_insert (struct list_elem *before, struct list_elem *elem) {
    elem->next = before->next;
    elem->prev = before;
    before->next->prev = elem;
    before->next = elem;
}

void
list_push_back (struct list *list, struct list_elem *elem) {
    list_insert (list->tail->prev, elem);
}

struct list_elem *
list_remove (struct list_elem *elem) {
    elem->next->prev = elem->prev;
    elem->prev->next = elem->next;
    return elem;
}




