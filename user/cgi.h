#ifndef CGI_H
#define CGI_H

#include <esp8266.h>
#include "httpd.h"

int tplCounter(HttpdConnData *connData, char *token, void **arg);

int cgiRandomNumbers(HttpdConnData *connData);

#endif
