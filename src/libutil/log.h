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

#ifndef LIBUTIL_LOG_H
#define LIBUTIL_LOG_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <libutil/likely.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


enum log_level {
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_TRACE 
};


typedef void (*custom_logger) (enum log_level v, const char *fmt,
                               va_list ap);

typedef void (*custom_exit) (int code);


typedef void (*custom_abort) ();



void
lu_printf(enum log_level v, const char *fmt, ...)
                                    __attribute__ ((format (printf, 2, 3)));

void
lu_exit(int exit_code);


void
lu_abort(void);


#define INFO(format, ...) \
  lu_printf(LOG_LEVEL_INFO, "INFO: " format "\n", ##__VA_ARGS__)


#define WARN(format, ...) \
  lu_printf(LOG_LEVEL_WARN, "WARN: " format "\n", ##__VA_ARGS__)


#define TRACE(format, ...) \
    lu_printf(LOG_LEVEL_TRACE, "TRACE: " format "\n", ##__VA_ARGS__)


#define ERROR(format, ...)                                              \
({                                                                      \
    lu_printf(LOG_LEVEL_ERROR, "ERROR: " format "\n", ##__VA_ARGS__);   \
    lu_exit(2);                                                         \
})


#define DIE(format, ...)                                                \
({                                                                      \
    lu_printf(LOG_LEVEL_FATAL, "FATAL: %s:%d: " format "\n",            \
              __FILE__, __LINE__, ##__VA_ARGS__);                       \
    lu_abort();                                                         \
})


#define CHECK(call, errcond, action, format, ...)                       \
({                                                                      \
    __typeof__(call) __ret = call;                                      \
    if (unlikely(__ret errcond))                                        \
    action("%s %s: " format, #call, #errcond, ##__VA_ARGS__);           \
    __ret;                                                              \
})


#define SYS(call)  CHECK(call, == -1, DIE, "sys call error : '%m'")


#define MEM(call)  CHECK(call, == NULL, DIE, "memory exhausted")


void set_logger(custom_logger);


void set_exit(custom_exit);


void set_abort(custom_abort);


#ifdef __cplusplus
} // extern "C"
#endif


#endif 

