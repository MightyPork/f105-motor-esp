#ifndef PAGE_WAVEFORM_H
#define PAGE_WAVEFORM_H

#include <httpd.h>

httpd_cgi_state tplWaveformJSON(HttpdConnData *connData, char *token, void **arg);

httpd_cgi_state tplFourierJSON(HttpdConnData *connData, char *token, void **arg);

#endif // PAGE_WAVEFORM_H
