#ifndef PAGE_MONITORING_H
#define PAGE_MONITORING_H

#include <httpd.h>

int tplMonitoring(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_MONITORING_H
