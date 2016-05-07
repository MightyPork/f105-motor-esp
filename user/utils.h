#ifndef UTILS_H
#define UTILS_H

#include <esp8266.h>

int rssi2perc(int rssi);
const char *auth2str(AUTH_MODE auth);
const char *opmode2str(WIFI_MODE opmode);

#endif // UTILS_H
