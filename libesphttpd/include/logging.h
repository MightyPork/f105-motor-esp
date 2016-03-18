#ifndef HTTPD_LOGGING_H
#define HTTPD_LOGGING_H


#ifndef httpd_printf
#define httpd_printf(fmt, ...) os_printf(fmt, ##__VA_ARGS__)
#endif


// logging functions
#define LOG_EOL "\n"
#define dbg(fmt, ...)    httpd_printf(fmt LOG_EOL, ##__VA_ARGS__);
#define banner(fmt, ...) httpd_printf("\x1b[32;1m"fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__);
#define info(fmt, ...)   httpd_printf("\x1b[32m"fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__);
#define error(fmt, ...)  httpd_printf("\x1b[31;1m"fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__);
#define warn(fmt, ...)   httpd_printf("\x1b[33;1m"fmt"\x1b[0m"LOG_EOL, ##__VA_ARGS__);


#endif // HTTPD_LOGGING_H
