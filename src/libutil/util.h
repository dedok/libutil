#ifndef LIBUTIL_UTIL_H
#define LIBUTIL_UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED_ARG(v) v __attribute__ ((unused))

#define MEM_CALLOC(type) (type *)MEM(calloc(1, sizeof(type)))

#define MEM_ALLOC(type) (type *)MEM(malloc(sizeof(type)))

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

#define DESIGNATE_MALLOC(type, ...) \
    DESIGNATE_ALLOC_(type, MEM_ALLOC, __VA_ARGS__)

#define DESIGNATE_CALLOC(type, ...) \
    DESIGNATE_ALLOC_(typem MEM_CALLOC, __VA_ARGS__)

#define STATIC_ASSERT(expr, message) \
    ##message[expr ? 1 : -1];

#define LU_SWAP(a, b)                           \
do {                                            \
    __typeof__(a) __a = (a);                    \
    (a) = (b);                                  \
    (b) = __a;                                  \
} while(0)

#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif

