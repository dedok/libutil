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

#ifndef LIBUTIL_ARCH_H
#define LIBUTIL_ARCH_H 1

#include <libutil/util.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
    All implementations from GNU sources
*/


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


#if defined(__APPLE__)

#include <time.h>
#include <mach/mach_time.h>

#define ARCH_TIME_NANO (+1.0E-9)
#define ARCH_TIME_GIGA UINT64_C(1000000000)
#define CLOCK_THREAD_CPUTIME_ID -1


inline static
void
clock_gettime(int UNUSED_VAR(f), struct timespec * ts)
{
    //XXX be more careful in a multithreaded environement
    static double timebase__ = 0.0;
    static uint64_t timestart__ = 0;

    if (!timestart__) {
        mach_timebase_info_data_t tb = { 0, 0 };
        mach_timebase_info(&tb);
        timebase__ = tb.numer;
        timebase__ /= tb.denom;
        timestart__ = mach_absolute_time();
    }

    double const diff =
        (mach_absolute_time() - timestart__) * timebase__;
    ts->tv_sec = diff * ARCH_TIME_NANO;
    ts->tv_nsec = diff - (ts->tv_sec * ARCH_TIME_GIGA);
}

#elif defined __lunix__
#include <time.h>
#endif


#ifdef __cplusplus
}      /* extern "C" */
#endif /* __cplusplus */


#endif

