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

