#ifndef PRINTK_H
#define PRINTK_H

int printk(const char *format, ...);

#define VERBOSE 0
#define DEBUG 1
#define INFO 2
#define WARN 3
#define RELEASE 15

#ifndef LOG_LEVEL
#define LOG_LEVEL RELEASE
#endif

#if LOG_LEVEL <= VERBOSE
#define VERBOSE_PRINTK(fmt, ...) printk("[ VERBOSE ] " fmt, ##__VA_ARGS__)
#else
#define VERBOSE_PRINTK(fmt, ...)
#endif

#if LOG_LEVEL <= DEBUG
#define DEBUG_PRINTK(fmt, ...) printk("[ DEBUG ] " fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINTK(fmt, ...)
#endif

#if LOG_LEVEL <= INFO
#define INFO_PRINTK(fmt, ...) printk("[ INFO ] " fmt, ##__VA_ARGS__)
#else
#define INFO_PRINTK(fmt, ...)
#endif

#if LOG_LEVEL <= WARN
#define WARN_PRINTK(fmt, ...) printk("[ WARN ] " fmt, ##__VA_ARGS__)
#else
#define WARN_PRINTK(fmt, ...)
#endif

#endif
