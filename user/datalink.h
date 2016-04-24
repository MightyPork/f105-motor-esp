#ifndef DATALINK_H
#define DATALINK_H

#include <esp8266.h>
#include <sbmp.h>

// request to capture data...
#define DG_REQUEST_RAW 40
#define DG_REQUEST_FFT 41
// reporting helpers
#define DG_REQUEST_STORE_REF 42
#define DG_REQUEST_COMPARE_REF 43


extern SBMP_Endpoint *dlnk_ep;

void datalinkInit(void);

void datalink_receive(uint8_t byte);

#endif // DATALINK_H
