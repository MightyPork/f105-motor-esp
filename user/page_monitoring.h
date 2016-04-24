#ifndef PAGE_MONITORING_H
#define PAGE_MONITORING_H

#include <httpd.h>

httpd_cgi_state tplMonitoring(HttpdConnData *connData, char *token, void **arg);

httpd_cgi_state cgiMonCompare(HttpdConnData *connData);

httpd_cgi_state cgiMonSetRef(HttpdConnData *connData);

httpd_cgi_state cgiMonitoringCfg(HttpdConnData *connData);

#endif // PAGE_MONITORING_H
