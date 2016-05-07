#ifndef DATALINK_H
#define DATALINK_H

#include <esp8266.h>
#include <sbmp.h>

// request to capture data...
#define DG_REQUEST_RAW 40
#define DG_REQUEST_FFT 41

// reporting
#define DG_REQUEST_STORE_REF 42 // request to capture & store reference FFT
#define DG_REQUEST_COMPARE_REF 43 // request to capture FFT & compare with reference

// wifi status & control
#define DG_SETMODE_AP 44 // request AP mode (AP button pressed)
#define DG_WPS_START 45 // start WPS
#define DG_WIFI_STATUS 46 // WiFi status report

#define DG_REQUEST_STM_VERSION 47 // Get acquisition module firmware version

extern SBMP_Endpoint *dlnk_ep;

void datalinkInit(void);

void datalink_receive(uint8_t byte);

#endif // DATALINK_H
