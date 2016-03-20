#ifndef UPTIME_H
#define UPTIME_H

#include <esp8266.h>

extern volatile uint32_t uptime;

void uptime_timer_init(void);

/** Print uptime to a buffer. Should be at least 20 long. */
void uptime_str(char *buf);

#endif // UPTIME_H
