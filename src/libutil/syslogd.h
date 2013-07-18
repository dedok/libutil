#ifndef LIBUTIL_SYSLOGD_H 
#define LIBUTIL_SYSLOGD_H 1

#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif

void
syslogd_enable(char const * ident, int log_facility);

#define syslogd_daemon_enable(ident) syslogd_enable(ident, LOG_DAEMON)

void 
syslogd_close(void);

#ifdef __cplusplus
}      /* extern "C" */
#endif

#endif

