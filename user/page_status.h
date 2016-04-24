#ifndef PAGE_HOME_H
#define PAGE_HOME_H

#include <httpd.h>

httpd_cgi_state tplSystemStatus(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_HOME_H
