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


#include "libutil/log.h"

static void
default_logger(enum log_level v, const char *fmt, va_list ap);


static custom_exit exit_ = exit;
static custom_abort abort_ = abort;
static custom_logger logger_ = default_logger;


static void
default_logger(enum log_level v, const char *fmt, va_list ap)
{
    vfprintf(v == LOG_LEVEL_INFO     ? stdout :
             v == LOG_LEVEL_WARN     ? stdout :
             v == LOG_LEVEL_TRACE    ? stdout :
             v == LOG_LEVEL_ERROR    ? stderr :
             v == LOG_LEVEL_FATAL    ? stderr :
             (DIE("unknown log_level %d", v), stderr),
             fmt, ap);
}


void
set_logger(custom_logger l)
{
    logger_ = l;
}


void
lu_printf(enum log_level v, const char *fmt, ...)
{
    if (logger_) {
        va_list ap; 
        va_start(ap, fmt);
        logger_(v, fmt, ap);
        va_end(ap);
    }
}


void
set_exit(custom_exit e)
{
    exit_ = e;
}


void
lu_exit(int code)
{
    exit_(code);
}


void
set_abort(custom_abort a)
{
    abort_ = a;
}


void
lu_abort(void)
{
    abort_(); 
}

