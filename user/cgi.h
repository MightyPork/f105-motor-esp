#ifndef CGI_H
#define CGI_H

#include <esp8266.h>
#include "httpd.h"

int tplReadSamplesJSON(HttpdConnData *connData, char *token, void **arg);

#endif
