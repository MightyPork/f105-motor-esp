#ifndef PAGE_WAVEFORM_H
#define PAGE_WAVEFORM_H

#include <httpd.h>

int tplWaveformJSON(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_WAVEFORM_H
