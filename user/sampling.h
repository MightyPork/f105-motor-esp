#ifndef SAMPLING_H
#define SAMPLING_H

#include <esp8266.h>
#include <httpd.h>

// temporary func to nread samples
int cgiReadSamples(HttpdConnData *connData);

#endif // SAMPLING_H
