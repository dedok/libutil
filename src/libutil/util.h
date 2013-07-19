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

#ifndef LIBUTIL_UTIL_H
#define LIBUTIL_UTIL_H 1


#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define UNUSED_VAR(v) v __attribute__ ((unused))


#define MEM_CALLOC(type) (type *)MEM(calloc(1, sizeof(type)))


#define MEM_ALLOC(type) (type *)MEM(malloc(sizeof(type)))


#define DESIGNATE_MALLOC(type, ...) \
    DESIGNATE_ALLOC_(type, MEM_ALLOC, __VA_ARGS__)


#define DESIGNATE_CALLOC(type, ...) \
    DESIGNATE_ALLOC_(typem MEM_CALLOC, __VA_ARGS__)


#define STATIC_ASSERT(expr, message) \
    char ##message[expr ? 1 : -1];


#define MEMBEROF(type, mem, from) \
    (((type*)(((unsigned char *)(from))-offsetof(type, mem))))


#define TYPE_CHECK(type, x)                     \
({                                              \
    type dummy__;                               \
    typeof(x) dummy2__;                         \
    (void)(&dummy__ == &dummy2__);              \
    1; \
})


#define DESIGNATE_ALLOC_(type, allocator, ...)  \
({                                              \
    type const t__ = { __VA_ARGS__ };           \
    type * x__ = allocator(type);               \
    memcpy(x__, &t__, sizeof(type));            \
    x;                                          \
})


#define SWAP(a, b)                              \
do {                                            \
    __typeof__(a) __a = (a);                    \
    (a) = (b);                                  \
    (b) = __a;                                  \
} while(0)


#ifdef __cplusplus
}      /* extern "C" */
#endif /* __cplusplus */


#endif

