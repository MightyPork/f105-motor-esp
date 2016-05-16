#ifndef PAGE_ABOUT_H
#define PAGE_ABOUT_H

#include <httpd.h>

httpd_cgi_state tplHome(HttpdConnData *connData, char *token, void **arg);
httpd_cgi_state FLASH_FN cgiMotorStop(HttpdConnData *connData);
httpd_cgi_state FLASH_FN cgiMotorStart(HttpdConnData *connData);

#endif // PAGE_ABOUT_H
