/*
    Copyright (C) 2011
*/

#ifndef LIBUTIL_SEQLOCK_H
#define LIBUTIL_SEQLOCK_H 1

#include <libutil/arch.h>
#include <libutil/likely.h>

typedef volatile unsigned int seqlock_t;

#define seqlock_init(lock)  lock = 0

#define seqlock_write_lock(lock)                \
do {                                            \
    ++(lock);                                   \
    wmb();                                      \
} while (0)

#define seqlock_write_unlock(lock)              \
do {                                            \
    wmb();                                      \
    ++(lock);                                   \
} while (0)

#define seqlock_read_lock(lock)                 \
do {                                            \
    seqlock_t _seqlock_lock_old = (lock);       \
    while (unlikely(_seqlock_lock_old & 0x1)) { \
        cpu_pause();                            \
        _seqlock_lock_old = (lock);             \
    }                                           \
    rmb()

#define seqlock_read_unlock(lock)              \
    rmb();                                     \
    if (likely(_seqlock_lock_old == (lock)))   \
        break;                                 \
} while (1)

#endif

