#ifndef CGI_H
#define CGI_H

#include <esp8266.h>
#include "httpd.h"

int tplHome(HttpdConnData *connData, char *token, void **arg);

//int cgiRandomNumbers(HttpdConnData *connData);

int tplMultipart(HttpdConnData *connData, char *token, void **arg);

#endif
