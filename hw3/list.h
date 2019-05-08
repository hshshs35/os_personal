struct list_elem {
    size_t size;
    int free;
    struct list_elem *prev;
    struct list_elem *next;
};

struct list {
    struct list_elem head;
    struct list_elem tail;
};

void
list_init (struct list *list);

void
list_insert (struct list_elem *before, struct list_elem *elem);

void
list_push_back (struct list *list, struct list_elem *elem);

struct list_elem *
list_remove (struct list_elem *elem);