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
#include "libutil/syslogd.h"

#include <stdarg.h>
#include <unistd.h>

static int log_facility_;

static void
syslogd_log_func(enum log_level v, const char *fmt, va_list ap)
{
    switch (v) {
        case LOG_LEVEL_INFO:
            vsyslog(LOG_MAKEPRI(log_facility_, LOG_INFO), fmt, ap);
        break;
        case LOG_LEVEL_WARN:
            vsyslog(LOG_MAKEPRI(log_facility_, LOG_WARNING), fmt, ap);
        break;
        case LOG_LEVEL_ERROR:
            vsyslog(LOG_MAKEPRI(log_facility_, LOG_ERR), fmt, ap);
        break;
        case LOG_LEVEL_FATAL:
            vsyslog(LOG_MAKEPRI(log_facility_, LOG_ERR), fmt, ap);
        break;
        case LOG_LEVEL_TRACE:
            vsyslog(LOG_MAKEPRI(log_facility_, LOG_INFO), fmt, ap);
        break;
        default : DIE("unknown log level"); break;
    }
}

void
syslogd_enable(char const * ident, int log_facility)
{
    log_facility_ = log_facility;
    openlog(ident, LOG_PID | LOG_CONS, 0);
    set_logger(syslogd_log_func);
}

void
syslogd_close(void)
{
    closelog();
}

