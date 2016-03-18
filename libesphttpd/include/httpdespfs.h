#ifndef HTTPDESPFS_H
#define HTTPDESPFS_H

#include "httpd.h"

/** Catch-all, use in '*' routes */
int cgiEspFsHook(HttpdConnData *connData);

/** Template route */
int ICACHE_FLASH_ATTR cgiEspFsTemplate(HttpdConnData *connData);

/** Static file route with the file as the first arg. */
int ICACHE_FLASH_ATTR cgiEspFsFile(HttpdConnData *connData);

#endif
