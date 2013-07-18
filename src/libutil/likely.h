/*
    Copyright (C)

    Define likely() and unlikely() macros.

*/

#ifndef LIBUTIL_LIKELY_H
#define LIBUTIL_LIKELY_H 1

#include <libutil/config.h>

#if defined(HAS_LIKELY_SUPPORT)

#define likely(expr)  __builtin_expect(!!(expr), 1)

#define unlikely(expr)  __builtin_expect(!!(expr), 0)

#else /* HAS_LIKELY_SUPPORT */

#define likely(expr) expr

#define unlikely(expr) expr

#endif /* LIBUTIL_LIKELY_H */

