#include "log.h"
#include "tinyswoole.h"
#include "tswoole_config.h"

void tswLog_put(int level, char *cnt)
{
    const char *level_str;
    char date_str[TSW_LOG_DATE_STRLEN];
    char log_str[TSW_LOG_BUFFER_SIZE];
    int n;
    time_t t;
    struct tm *p;

    switch (level) {
        case TSW_LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case TSW_LOG_NOTICE:
            level_str = "NOTICE";
            break;
        case TSW_LOG_ERROR:
            level_str = "ERROR";
            break;
        case TSW_LOG_WARNING:
            level_str = "WARNING";
            break;
        case TSW_LOG_TRACE:
            level_str = "TRACE";
            break;
        default:
            level_str = "INFO";
            break;
    }

    t = time(NULL);
    p = localtime(&t);
    snprintf(date_str, TSW_LOG_DATE_STRLEN, "%d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    n = snprintf(log_str, TSW_LOG_BUFFER_SIZE, "[%s]\t%s\t%s\n", date_str, level_str, cnt);
    if (write(STDOUT_FILENO, log_str, n) < 0) {
        printf("write(log_fd, size=%d) failed. Error: %s[%d].\n", n, strerror(errno), errno);
    }
}
