/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

// library headers
#include <esp8266.h>
#include "httpd.h"
#include "httpdespfs.h"
#include "cgiwifi.h"
#include "cgiflash.h"
#include "auth.h"
#include "espfs.h"
#include "captdns.h"
#include "webpages-espfs.h"
#include "cgiwebsocket.h"

// user files
#include "cgi.h"
#include "serial.h"
#include "io.h"
#include "datalink.h"
#include "uart_driver.h"
#include "uptime.h"

#include "page_home.h"
#include "sampling.h"

#include "sbmp.h"

static ETSTimer prHeapTimer;

/** Timer called each second */
static void ICACHE_FLASH_ATTR prHeapTimerCb(void *arg)
{
	static u8 cnt = 0;
	static u32 last = 0;

	if (++cnt == 3) {
		cnt = 0;
		u32 heap = system_get_free_heap_size();
		dbg("Heap: %u (~ %d)", heap, (heap-last));
		last = heap;
	}

	// we will also try to set up a SBMP connection
	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		sbmp_ep_start_handshake(dlnk_ep);
	}
}


/**
 * @brief BasicAuth name/password checking function.
 *
 * It's invoked by the authBasic() built-in route handler
 * until it returns 0. Each time it populates the provided
 * name and password buffer.
 *
 * @param connData : connection context
 * @param no       : user number (incremented each time it's called)
 * @param user     : user buffer
 * @param userLen  : user buffer size
 * @param pass     : password buffer
 * @param passLen  : password buffer size
 * @return 0 to end, 1 if more users are available.
 */
int FLASH_FN myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen)
{
	(void)connData;
	(void)userLen;
	(void)passLen;

	if (no == 0) {
		os_strcpy(user, "admin");
		os_strcpy(pass, "s3cr3t");
		return 1;
//Add more users this way. Check against incrementing no for each user added.
//  } else if (no==1) {
//      os_strcpy(user, "user1");
//      os_strcpy(pass, "something");
//      return 1;
	}
	return 0;
}


// Some stuff for alternative ESPFS storage methods
#ifdef ESPFS_POS
CgiUploadFlashDef uploadParams = {
	.type = CGIFLASH_TYPE_ESPFS,
	.fw1Pos = ESPFS_POS,
	.fw2Pos = 0,
	.fwSize = ESPFS_SIZE,
};
#define INCLUDE_FLASH_FNS
#endif
#ifdef OTA_FLASH_SIZE_K
CgiUploadFlashDef uploadParams = {
	.type = CGIFLASH_TYPE_FW,
	.fw1Pos = 0x1000,
	.fw2Pos = ((OTA_FLASH_SIZE_K * 1024) / 2) + 0x1000,
	.fwSize = ((OTA_FLASH_SIZE_K * 1024) / 2) - 0x1000,
	.tagName = OTA_TAGNAME
};
#define INCLUDE_FLASH_FNS
#endif


/**
 * This is the main url->function dispatching data struct.
 *
 * In short, it's a struct with various URLs plus their handlers. The handlers can
 * be 'standard' CGI functions you wrote, or 'special' CGIs requiring an argument.
 * They can also be auth-functions. An asterisk will match any url starting with
 * everything before the asterisks; "*" matches everything. The list will be
 * handled top-down, so make sure to put more specific rules above the more
 * general ones. Authorization things (like authBasic) act as a 'barrier' and
 * should be placed above the URLs they protect.
 */


static HttpdBuiltInUrl builtInUrls[] = {
	ROUTE_CGI_ARG("*", cgiRedirectApClientToHostname, "esp8266.nonet"), // redirect func for the captive portal

	ROUTE_TPL_FILE("/", tplHome, "/pages/home.tpl"),

	ROUTE_CGI("/acquire.cgi", cgiReadSamples),

	ROUTE_TPL_FILE("/multipart", tplMultipart, "/multipart.tpl"),

//Enable the line below to protect the WiFi configuration with an username/password combo.
//  ROUTE_AUTH("/wifi/*", myPassFn),

	ROUTE_REDIRECT("/wifi/",  "/wifi"),

	ROUTE_TPL_FILE("/wifi", tplWlan, "/pages/wifi.tpl"),

	ROUTE_CGI("/wifi/scan.cgi", cgiWiFiScan),
	ROUTE_CGI("/wifi/connect.cgi", cgiWiFiConnect),
	ROUTE_CGI("/wifi/connstatus.cgi", cgiWiFiConnStatus),
	ROUTE_CGI("/wifi/setmode.cgi", cgiWiFiSetMode),

	ROUTE_FS("*"), //Catch-all cgi function for the filesystem

	ROUTE_END()
};


//static ETSTimer prTestTimer;

//static void ICACHE_FLASH_ATTR test_timer_task(void *arg) {
//	const char * t = "Test\r\n";
//	UART_WriteBuffer(0, (uint8_t*)t, strlen(t), 1000);
//}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/**
 * Main routine. Initialize stdout, the I/O, filesystem and the webserver and we're done.
 */
void user_init(void)
{
	// set up the debuging output
	serialInit();
	uptime_timer_init();

	banner("\n*** ESP8266 starting, "
			  "HTTPD v."HTTPDVER", "
			  "SBMP v."SBMP_VER", "
			  "IoT SDK v." STR(ESP_SDK_VERSION)" ***\n");

	// reset button etc
	ioInit();

	// set up SBMP
	datalinkInit();

	// Start the captive portal
	captdnsInit();

	// 0x40200000 is the base address for spi flash memory mapping, ESPFS_POS is the position
	// where image is written in flash that is defined in Makefile.
#ifdef ESPFS_POS
	espFsInit((void*)(0x40200000 + ESPFS_POS));
#else
	espFsInit((void*)(webpages_espfs_start));
#endif

	/* --- Initialize the webserver --- */

	httpdInit(builtInUrls, 80);

	info("\nReady\n");

	os_timer_disarm(&prHeapTimer);
	os_timer_setfn(&prHeapTimer, prHeapTimerCb, NULL);
	os_timer_arm(&prHeapTimer, 1000, 1);
}


void user_rf_pre_init()
{
	//Not needed, but some SDK versions want this defined.
}
