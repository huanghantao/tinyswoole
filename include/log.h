#ifndef LOG_H_
#define LOG_H_


#define TSW_LOG_BUFFER_SIZE 1024
#define TSW_LOG_DATE_STRLEN  64

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