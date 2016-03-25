#ifndef HTTPD_LOGGING_H
#define HTTPD_LOGGING_H

#include <esp8266.h>
#include "uptime.h"


// logging functions
#define LOG_EOL "\n"


#define dbg(fmt, ...) \
	do { \
		uptime_print(); \
		printf(" [ ] "fmt LOG_EOL, ##__VA_ARGS__); \
	} while(0)


#define banner(fmt, ...) \
	do {  \
		printf("\x1b[32;1m");  \
		uptime_print();  \
		printf(" [i] "fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__);  \
	} while(0)


#define info(fmt, ...) \
	do { \
		printf("\x1b[32m"); \
		uptime_print(); \
		printf(" [i] "fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__); \
	} while(0)


#define error(fmt, ...) \
	do { \
		printf("\x1b[31;1m"); \
		uptime_print(); \
		printf(" [E] "fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__); \
	} while(0)


#define warn(fmt, ...) \
	do { \
		printf("\x1b[33;1m"); \
		uptime_print(); \
		printf(" [W] "fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__); \
	} while(0)


#endif // HTTPD_LOGGING_H
