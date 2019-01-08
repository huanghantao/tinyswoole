#ifndef LOG_H_
#define LOG_H_


#define TSW_LOG_BUFFER_SIZE 1024
#define TSW_LOG_DATE_STRLEN  64

#define TSW_DEBUG_MSG_SIZE 512
#define TSW_ERROR_MSG_SIZE 512

char tsw_debug[TSW_DEBUG_MSG_SIZE];
char tsw_error[TSW_ERROR_MSG_SIZE];

#define tswDebug(str, ...)                                                         \
	snprintf(tsw_debug, TSW_DEBUG_MSG_SIZE, str, ##__VA_ARGS__); \
	tswLog_put(TSW_LOG_DEBUG, tsw_debug);
#define tswWarn(str, ...)                                                         \
	snprintf(tsw_error, TSW_ERROR_MSG_SIZE, "%s: "str" in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
	tswLog_put(TSW_LOG_WARNING, tsw_error);

enum tswLog_level
{
	TSW_LOG_DEBUG = 0,
	TSW_LOG_TRACE,
	TSW_LOG_INFO,
	TSW_LOG_NOTICE,
	TSW_LOG_WARNING,
	TSW_LOG_ERROR,
};

void tswLog_put(int level, char *cnt);

#endif /* LOG_H_ */