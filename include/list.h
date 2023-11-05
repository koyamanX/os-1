#ifndef LIST_H
#define LIST_H

typedef struct list_elem {
    struct list_elem *prev;
    struct list_elem *next;
} list_elem_t;

typedef list_elem_t list_t;

#define LIST_INIT(list) { &(list), &(list) }

#define LIST_ENTRY(elem, type, member) \
    ((type *)(void *)(elem) - offsetof(type, member))

#define LIST_EMPTY(list) \
    ((list)->next == (list))

#define LIST_FOREACH(list, elem, next_elem) \
    for (elem = (list)->next, next_elem = elem->next; \
         elem != (list); \
         elem = next_elem, next_elem = elem->next)

void list_init(list_t *list);
void list_push_back(list_t *list, list_elem_t *elem);
list_elem_t *list_pop_front(list_t *list);

#endif // LIST_H