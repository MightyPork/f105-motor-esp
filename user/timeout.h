#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <esp8266.h>

#define until_timeout(to_ms) for(uint32_t _utmeo = system_get_time(); system_get_time() - _utmeo < ((to_ms)*1000);)

/** Retry a call until a timeout. Variable 'suc' is set to the return value. Must be defined. */
#define retry_TO(to_ms, call)  \
	until_timeout(to_ms) { \
		suc = call; \
		if (suc) break; \
	}

#endif // TIMEOUT_H
