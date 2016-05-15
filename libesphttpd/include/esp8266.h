// Combined include file for esp8266


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "espmissingprotos.h"

#ifdef FREERTOS
#include <stdint.h>
#include <espressif/esp_common.h>

#else
#include <c_types.h>

#include <ip_addr.h>
#include <espconn.h>
#include <ets_sys.h>
#include <gpio.h>
#include <mem.h>
#include <osapi.h>
#include <user_interface.h>
#include <upgrade.h>
#endif

#define FLASH_FN ICACHE_FLASH_ATTR

// this ideally would allow flash arrays, but alas works only for uint32_t
#define FLASH_DATA ICACHE_RODATA_ATTR STORE_ATTR

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#include "platform.h"
#include "esp_sdk_ver.h"

#include "logging.h"
