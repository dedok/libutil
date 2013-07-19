/*
  Copyright (C) 2010, 2012 Tomash Brechko.  All rights reserved.

  This header is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This header is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this header.  If not, see <http://www.gnu.org/licenses/>.


  DESCRIPTION:

  Implement doubly-linked list.

  Macros list_each() and list_each_backward() have a shadow iterator,
  so for instance it is safe to write

    struct mystruct
    {
      ...
      struct list_node mynode;
      ...
    };
    struct mystruct *mylist;

    ...

    for (struct mystruct *list_each(it, mylist, mynode))
      free(it);

  i.e. to modify *it in any way as it won't be referenced again during
  traversal.

  Defining KROKI_LIST_NOPOLLUTE will result in omitting alias
  definitions, but functionality will still be available with the
  namespace prefix 'kroki_'.
*/

#include <libutil/push_options.h>

#ifndef KROKI_LIST_NOPOLLUTE

#define list_node  kroki_list_node
#define LIST_INITIALIZER(head)  KROKI_LIST_INITIALIZER(head)
#define list_init(head)  kroki_list_init(head)
#define list_empty(head)  kroki_list_empty(head)
#define list_insert(head, node)  kroki_list_insert(head, node)
#define list_insert_first(head, node)  kroki_list_insert_first(head, node)
#define list_remove(node)  kroki_list_remove(node)
#define list_next(node, type, memb)  kroki_list_next(node, type, memb)
#define list_prev(node, type, memb)  kroki_list_prev(node, type, memb)
#define list_end(p, head, memb)  kroki_list_end(p, head, memb)
#define list_each(p, head, memb)  kroki_list_each(p, head, memb)
#define list_each_backward(p, head, memb)       \
  kroki_list_each_backward(p, head, memb)

#endif  /* ! KROKI_LIST_NOPOLLUTE */


#ifndef KROKI_LIST_H
#define KROKI_LIST_H 1

#include <stddef.h>


struct kroki_list_node
{
  struct kroki_list_node *next;
  struct kroki_list_node *prev;
};


#define KROKI_LIST_INITIALIZER(head)            \
  { &(head), &(head) }

#define kroki_list_init(head)                   \
  do                                            \
    (head)->prev = (head)->next = (head);       \
  while (0)

#define kroki_list_empty(head)                  \
  ((head)->next == (head))

#define kroki_list_insert(head, node)                   \
  do                                                    \
    {                                                   \
      struct kroki_list_node *_kroki_h = (head);        \
      struct kroki_list_node *_kroki_n = (node);        \
      _kroki_h->prev->next = _kroki_n;                  \
      _kroki_n->prev = _kroki_h->prev;                  \
      _kroki_h->prev = _kroki_n;                        \
      _kroki_n->next = _kroki_h;                        \
    }                                                   \
  while (0)

#define kroki_list_insert_first(head, node)     \
  kroki_list_insert((head)->next, node)

#define kroki_list_remove(node)                         \
  do                                                    \
    {                                                   \
      struct kroki_list_node *_kroki_n = (node);        \
      _kroki_n->prev->next = _kroki_n->next;            \
      _kroki_n->next->prev = _kroki_n->prev;            \
    }                                                   \
  while (0)

#define kroki_list_next(node, type, memb)               \
  _kroki_list_node2data((node)->next, type, memb)

#define kroki_list_prev(node, type, memb)               \
  _kroki_list_node2data((node)->prev, type, memb)

#define kroki_list_end(p, head, memb)           \
  (&(p)->memb == (head))

#define kroki_list_each(p, head, memb)                                  \
  _kroki_list_each(p, head, memb, next, _kroki_list_label(__COUNTER__))

#define kroki_list_each_backward(p, head, memb)                         \
  _kroki_list_each(p, head, memb, prev, _kroki_list_label(__COUNTER__))


#define _kroki_list_label(unique)  _kroki_list_label2(unique)

#define _kroki_list_label2(unique)  _kroki_list_label_##unique

#define _kroki_list_each(p, head, memb, dir, label)                     \
  /* for ([type *] */ (p) = NULL; ; ({ goto label; }))                  \
       if (0)                                                           \
         {                                                              \
         label:                                                         \
           break;                                                       \
         }                                                              \
       else                                                             \
         for (struct kroki_list_node *_kroki_list_it = (head)->dir;     \
              (((p) = _kroki_list_node2data(_kroki_list_it,             \
                                            __typeof__(*(p)), memb)),   \
               (_kroki_list_it != (head)                                \
                && ((_kroki_list_it = _kroki_list_it->dir), 1)));       \
              /* nothing */ /* ) */

#define _kroki_list_node2data(node, type, memb)         \
  ((type *) ((char *) (node) - offsetof(type, memb)))


#endif  /* ! KROKI_LIST_H */
