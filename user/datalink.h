#ifndef DATALINK_H
#define DATALINK_H

#include <esp8266.h>
#include <sbmp.h>

// request to capture data...
#define DG_REQUEST_CAPTURE 40


extern SBMP_Endpoint *dlnk_ep;

void datalinkInit(void);

void datalink_receive(uint8_t byte);

#endif // DATALINK_H
