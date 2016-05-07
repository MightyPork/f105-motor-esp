#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <esp8266.h>
#include <httpd.h>

void wificontrol_setmode_ap(void);

void wificontrol_start_wps(void);

void wificontrol_init(void);

#endif // WIFICONTROL_H
