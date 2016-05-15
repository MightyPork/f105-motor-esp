#ifndef PAGE_ABOUT_H
#define PAGE_ABOUT_H

#include <httpd.h>

httpd_cgi_state tplHome(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_ABOUT_H
