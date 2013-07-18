/*
    The MIT License (MIT)

    Copyright (c) 2013 Vasiliy Soshnikov

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef LIBUTIL_LIST_H
#define LIBUTIL_LIST_H 1

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct list_node
{
    struct list_node * next;
    struct list_node * prev;
};

inline void
list_init(struct list_node *head)
    { head->prev = head->next = head; }

inline bool
list_empty(const struct list_node *head)
    { return (head->next == head); }

inline void
list_insert(struct list_node *head, struct list_node *node)
{
    head->prev->next = node;
    node->prev = head->prev;
    head->prev = node;
    node->next = head;
}


inline void
list_insert_first(struct list_node *head, struct list_node *node)
    { list_insert(head->next, node); }

inline void
list_remove(const struct list_node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

#define LIST_INIT(head)                         \
  { .next = &head, .prev = &head }

#define list_next(node, type, memb)             \
  _list_node2data((node)->next, type, memb)

#define list_prev(node, type, memb)             \
  _list_node2data((node)->prev, type, memb)

#define list_end(head, p, memb)                 \
  (&(p)->memb == (head))

#define list_each(p, head, memb)                                \
  _list_each(p, head, memb, next, _list_label(__LINE__))

#define list_each_backward(p, head, memb)                       \
  _list_each(p, head, memb, prev, _list_label(__LINE__))

#define _list_label(unique)  _list_label2(unique)

#define _list_label2(unique)  _list_label_##unique

#define _list_each(p, head, memb, dir, label)                   \
  /* for ([type *] */ (p) = NULL; ; ({ goto label; }))          \
       if (false)                                               \
         {                                                      \
         label:                                                 \
           break;                                               \
         }                                                      \
       else                                                     \
         for (struct list_node *_list_it = (head)->dir;         \
              (((p) = _list_node2data(_list_it,                 \
                                      __typeof__(*(p)), memb)), \
               (_list_it != (head)                              \
                && ((_list_it = _list_it->dir), true)));        \
              /* nothing */ /* ) */

#define _list_node2data(node, type, memb)        \
  ((type *) ((char *) (node) - offsetof(type, memb)))

#ifdef __cplusplus
} // extern "C"     
#endif 

#endif

