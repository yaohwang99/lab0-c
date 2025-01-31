#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    head->next = head;
    head->prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry;
    element_t *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        list_del_init(&entry->list);
        q_release_element(entry);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    if (!s)
        return false;
    char *tmp = strdup(s);
    if (!tmp)
        return false;
    element_t *n = malloc(sizeof(element_t));
    if (!n) {
        free(tmp);
        return false;
    }
    n->value = tmp;
    list_add(&n->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    if (!s)
        return false;
    char *tmp = strdup(s);
    if (!tmp)
        return false;
    element_t *n = malloc(sizeof(element_t));
    if (!n) {
        free(tmp);
        return false;
    }
    n->value = tmp;
    list_add_tail(&n->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    list_del_init(&tmp->list);
    if (!sp)
        return tmp;
    size_t len = strlen(tmp->value);
    len = len > bufsize - 1 ? bufsize - 1 : len;
    strncpy(sp, tmp->value, len);
    sp[len] = '\0';
    return tmp;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    list_del_init(&tmp->list);
    if (!sp)
        return tmp;
    size_t len = strlen(tmp->value);
    len = len > bufsize - 1 ? bufsize - 1 : len;
    strncpy(sp, tmp->value, len);
    sp[len] = '\0';
    return tmp;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}


/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;
    if (list_empty(head))
        return false;
    element_t *front = list_entry(head->next, element_t, list),
              *back = list_entry(head->prev, element_t, list);
    for (; front != back && front->list.next != &back->list;
         front = list_entry(front->list.next, element_t, list),
         back = list_entry(back->list.prev, element_t, list)) {
    }

    list_del_init(&back->list);
    q_release_element(back);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head)
        return false;
    if (list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;
    struct list_head *rec = q_new();
    struct list_head *front = head->next;
    struct list_head *back = head->next->next;
    while (front != head && back != head) {
        element_t *back_entry = list_entry(back, element_t, list);
        element_t *front_entry = list_entry(front, element_t, list);
        while (strcmp(front_entry->value, back_entry->value) == 0) {
            back = back->next;
            if (back == head)
                break;
            back_entry = list_entry(back, element_t, list);
        }
        if (front->next != back) {
            struct list_head *prev = front->prev;

            rec->next->prev = back->prev;
            back->prev->next = rec->next;
            front->prev = rec;
            rec->next = front;

            prev->next = back;
            back->prev = prev;
        }

        front = back;
        back = back->next;
    }
    q_free(rec);
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    if (list_empty(head))
        return;
    if (list_is_singular(head))
        return;
    struct list_head *walk = head;
    while (walk->next != head && walk->next->next != head) {
        struct list_head *front = walk->next;
        struct list_head *back = walk->next->next;
        list_del_init(front);
        list_del_init(back);
        list_add(front, walk);
        list_add(back, walk);
        walk = walk->next->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head))
        return;
    if (list_is_singular(head))
        return;
    struct list_head *curr = head;
    struct list_head *prev = head->prev;
    struct list_head *next = head->next;
    do {
        curr->prev = next;
        curr->next = prev;
        prev = curr;
        curr = next;
        next = next->next;
    } while (curr != head);
}
void my_merge(struct list_head **li,
              struct list_head **mi,
              struct list_head **ri)
{
    struct list_head *l_start = *li;
    struct list_head *l_end = *mi;
    struct list_head *r_start = (*mi)->next;
    struct list_head *r_end = *ri;
    struct list_head *walk = (*li)->prev;

    struct list_head *head = (*li)->prev;
    struct list_head *tail = (*ri)->next;

    element_t *l_entry = list_entry(l_start, element_t, list);
    element_t *r_entry = list_entry(r_start, element_t, list);
    while (true) {
        if (strcmp(l_entry->value, r_entry->value) > 0) {
            struct list_head *next_r_start = r_start->next;
            list_del_init(r_start);
            list_add(r_start, walk);

            walk = walk->next;
            if (r_start == r_end) {
                break;
            }

            r_start = next_r_start;
            r_entry = list_entry(r_start, element_t, list);
        } else {
            struct list_head *next_l_start = l_start->next;
            list_del_init(l_start);
            list_add(l_start, walk);
            walk = walk->next;
            if (l_start == l_end) {
                break;
            }

            l_start = next_l_start;
            l_entry = list_entry(l_start, element_t, list);
        }
    }
    *li = head->next;
    *ri = tail->prev;
}
void merge_sort(struct list_head **li, struct list_head **ri)
{
    if (*li == *ri)
        return;
    struct list_head *front = *li, *back = *ri;
    for (; front != back && front->next != back;
         front = front->next, back = back->prev) {
    }
    merge_sort(li, &back->prev);
    merge_sort(&back, ri);
    struct list_head **mi = &back->prev;
    my_merge(li, mi, ri);
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head)
        return;
    merge_sort(&head->next, &head->prev);
}
