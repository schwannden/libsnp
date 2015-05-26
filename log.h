#ifndef __log_h__
#define __log_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define LOG_LEVEL_ALL   5
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_NONE  0

// set default log level to info
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

//deleting the unwanted log
#if LOG_LEVEL < LOG_LEVEL_DEBUG
#define log_debug(M, ...)
#endif
#if LOG_LEVEL < LOG_LEVEL_INFO
#define log_info(M, ...)
#endif
#if LOG_LEVEL < LOG_LEVEL_WARN
#define log_warn(M, ...)
#endif
#if LOG_LEVEL < LOG_LEVEL_ERROR
#define log_error(M, ...)
#endif

//generating wanted log
#ifndef log_debug
#define log_debug(M, ...) fprintf(stderr, "[DEBUG] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#ifndef log_info
#define log_info(M, ...)  fprintf(stderr, "[INFO]  (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#ifndef log_warn
#define log_warn(M, ...)  fprintf(stderr, "[WARN]  (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#ifndef log_error
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, strerror(errno), ##__VA_ARGS__)
#endif

#endif
