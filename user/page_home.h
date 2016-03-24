#ifndef PAGE_HOME_H
#define PAGE_HOME_H

#include <httpd.h>

int tplHome(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_HOME_H
