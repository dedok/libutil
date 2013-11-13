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

#define SYSLOG_NAMES 1
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>


static struct {
    int sfd;
} syslogd_desc__ = { -1 };


static int SYSLOG_INFO__ = 0;
static int SYSLOG_WARNING__ = 0;
static int SYSLOG_ERROR__ = 0;
static int SYSLOG_FATAL__ = 0;
static int SYSLOG_TRACE__ = 0;


char syslogd_tag__[255];
char syslogd_pid__[30];


enum {
    TYPE_UDP = (1 << 1),
    TYPE_TCP = (1 << 2),
    ALL_TYPES = TYPE_UDP | TYPE_TCP
};


static
void
vsyslogf(enum log_level v, const char *fmt, va_list ap)
{
    switch (v) {
        case LOG_LEVEL_INFO: case LOG_LEVEL_TRACE:
            vsyslog(SYSLOG_INFO__, fmt, ap);
            break;
        case LOG_LEVEL_WARN:
            vsyslog(SYSLOG_WARNING__, fmt, ap);
            break;
        case LOG_LEVEL_ERROR: case LOG_LEVEL_FATAL:
            vsyslog(SYSLOG_ERROR__, fmt, ap);
            break;
        default : DIE("unknown log level"); break;
    }
}


static
void
vslogf_rfc3164(enum log_level v, const char *fmt, va_list ap)
{
    // TODO optimize *sprintf message formating
    enum { max_buf_size = 1024 };
    time_t now;
    int pri = SYSLOG_INFO__, rc;
    char buf[256 /* head */+ max_buf_size], msg[max_buf_size], * tp;

    switch (v) {
        case LOG_LEVEL_TRACE: case LOG_LEVEL_INFO:
            pri = SYSLOG_INFO__;
            break;
        case LOG_LEVEL_WARN:
            pri = SYSLOG_WARNING__;
            break;
        case LOG_LEVEL_FATAL: case LOG_LEVEL_ERROR:
            pri = SYSLOG_ERROR__;
            break;
        default : DIE("unknown log level"); break;
    }

    vsnprintf(msg, max_buf_size, fmt, ap);
    time(&now);
    tp = ctime(&now) + 4;
    rc = snprintf(buf, sizeof(buf),
            "<%d>%.15s %.200s%s: %.400s",
             pri, tp, syslogd_tag__, syslogd_pid__, msg);

    if (unlikely(syslogd_desc__.sfd != -1
        && write(syslogd_desc__.sfd, buf, rc + 1/* + \0 */) <= 0))
    {
        close(syslogd_desc__.sfd);
        syslogd_desc__.sfd = -1;
    }

    if (unlikely(syslogd_desc__.sfd == -1))
        fprintf(stderr, "%s", buf);
}


static inline
void
open_inet_socket(struct syslogd_options const * opt,
    int socket_type)
{
    short step = 2;
    struct addrinfo hints, *res;

    for (; step; step--) {
        memset(&hints, 0, sizeof(hints));
        if (step == 2 && socket_type & TYPE_UDP)
            hints.ai_socktype = SOCK_DGRAM;
        if (step == 1 && socket_type & TYPE_TCP)
            hints.ai_socktype = SOCK_STREAM;
        if (hints.ai_socktype == 0)
            continue;
        hints.ai_family = AF_UNSPEC;

        SYS(getaddrinfo(opt->server.host, opt->server.port, &hints, &res));
        if ((syslogd_desc__.sfd =
                    socket(res->ai_family, res->ai_socktype, res->ai_protocol))
                == -1)
        {
            freeaddrinfo(res);
            continue;
        }
        if (connect(syslogd_desc__.sfd,
            res->ai_addr, res->ai_addrlen) == -1)
        {
            freeaddrinfo(res);
            close(syslogd_desc__.sfd);
            continue;
        }

        freeaddrinfo(res);
        break;
  }

  CHECK(step, == 0, DIE, "failed to connect to %s port %s",
          opt->server.host, opt->server.port);
}


static inline
void
open_unix_socket(struct syslogd_options const * opt, int socket_type)
{
    int st = -1;
    short step = 2;
    static struct sockaddr_un s_addr;

    CHECK(strlen(opt->unix_socket), >= sizeof(s_addr.sun_path),
        DIE, "openlog %s: pathname too long", opt->unix_socket);

    s_addr.sun_family = AF_UNIX;
    strcpy(s_addr.sun_path, opt->unix_socket);

    for (step = 2; step; step--, st = -1) {
        if (step == 2 && socket_type & TYPE_UDP)
            st = SOCK_DGRAM;
        if (step == 1 && socket_type & TYPE_TCP)
            st = SOCK_STREAM;
        if (st == -1
            || (syslogd_desc__.sfd = socket(AF_UNIX, st, 0)) == -1)
            continue;

        if (connect(syslogd_desc__.sfd,
                    (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
            close(syslogd_desc__.sfd);
            continue;
        }

        break;
    }

    CHECK(step, == 0, DIE, "failed to connect to socket '%s'",
            opt->unix_socket);
}


void
syslogd_enable(char const * ident, int log_facility,
        struct syslogd_options const * opt)
{
    // default rsyslogd
    if (opt == NULL) {
        openlog(ident, LOG_PID | LOG_CONS, 0);
        set_logger(vsyslogf);
        return;
    }

    // used configured rsyslogd
    bzero(syslogd_tag__, sizeof(syslogd_tag__));
    bzero(syslogd_pid__, sizeof(syslogd_pid__));

    snprintf(syslogd_pid__, sizeof(syslogd_pid__), "[%d]", getpid());
    if (getlogin_r(&syslogd_tag__[0], sizeof(syslogd_tag__)) < 0) {
        switch (errno) {
            case ENXIO: case ERANGE: case ENOTTY:
                errno = 0;
                break;
            default : DIE("getlogin_r error '%m'");
        }
    }

    SYSLOG_INFO__ = SYSLOG_TRACE__ =
        ((log_facility & LOG_FACMASK) | (LOG_INFO & LOG_PRIMASK));
    SYSLOG_WARNING__ =
        ((log_facility & LOG_FACMASK) | (LOG_WARNING & LOG_PRIMASK));
    SYSLOG_ERROR__ = SYSLOG_FATAL__ =
        ((log_facility & LOG_FACMASK) | (LOG_ERR & LOG_PRIMASK));

    if (opt->unix_socket)
        open_unix_socket(opt, ALL_TYPES);
    else
        open_inet_socket(opt, ALL_TYPES);

    set_logger(vslogf_rfc3164);
}


void
syslogd_close(void)
{
    if (syslogd_desc__.sfd != -1)
        close(syslogd_desc__.sfd);
    else
        closelog();
}


#undef SYSLOG_NAMES

