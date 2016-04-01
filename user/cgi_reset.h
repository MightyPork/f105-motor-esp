#ifndef CGI_RESET_H
#define CGI_RESET_H

#include <esp8266.h>
#include <httpd.h>

int cgiResetDevice(HttpdConnData *connData);

#endif // CGI_RESET_H
