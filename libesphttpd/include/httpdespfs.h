#ifndef HTTPDESPFS_H
#define HTTPDESPFS_H

#include "httpd.h"

/** Catch-all, use in '*' routes */
httpd_cgi_state cgiEspFsHook(HttpdConnData *connData);

/** Template route */
httpd_cgi_state ICACHE_FLASH_ATTR cgiEspFsTemplate(HttpdConnData *connData);

/** Static file route with the file as the first arg. */
httpd_cgi_state ICACHE_FLASH_ATTR cgiEspFsFile(HttpdConnData *connData);

#endif
