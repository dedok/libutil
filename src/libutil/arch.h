#ifndef LIBUTIL_ARCH_H
#define LIBUTIL_ARCH_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define mb() \
    __asm__ __volatile__ ("mfence":::"memory")

#define rmb() \
    __asm__ __volatile__ ("lfence":::"memory")

#define wmb() \
    __asm__ __volatile__ ("sfence":::"memory")

#define cpu_pause() \
    __asm__ __volatile__ ("pause")

inline unsigned long
rdtsc(void)
{
    unsigned int tickl, tickh;
    __asm__ __volatile__ ("rdtsc":"=a"(tickl),"=d"(tickh));
    return ((unsigned long) tickh << 32) | tickl;
}

#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif

