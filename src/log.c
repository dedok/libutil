/*
  Copyright (C)
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
tg_abort(void)
{
    abort_(); 
}

