#include "routes.h"

#include <esp8266.h>
#include <httpd.h>

#include "httpdespfs.h"
#include "cgiwifi.h"
//#include "cgiflash.h"
//#include "auth.h"
//#include "cgiwebsocket.h"

// user files
#include "page_status.h"
#include "page_waveform.h"

#define WIFI_PROTECT 0

#if WIFI_PROTECT
static int FLASH_FN myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen);
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
HttpdBuiltInUrl builtInUrls[] = {
	ROUTE_CGI_ARG("*", cgiRedirectApClientToHostname, "esp8266.nonet"), // redirect func for the captive portal

	// ! Templates in the JSON folder will have application/json content type !
	// otherwise they're encoded the same like the HTML ones -> no heatshrink, no gzip

	// --- UI pages ---

	// System Status page
	ROUTE_TPL_FILE("/",        tplSystemStatus, "/pages/status.tpl"),
	ROUTE_TPL_FILE("/status",  tplSystemStatus, "/pages/status.tpl"),
	ROUTE_TPL_FILE("/api/status.json", tplSystemStatus, "/json/status.tpl"),

	// Waveform page
	ROUTE_FILE("/waveform", "/pages/waveform.html"), // static file, html -> can use gzip
	ROUTE_TPL_FILE("/api/raw.json", tplWaveformJSON, "/json/samples.tpl"),

	// --- WiFi config ---

#if WIFI_PROTECT
	ROUTE_AUTH("/wifi/*", myPassFn),
#endif

	ROUTE_TPL_FILE("/wifi", tplWlan, "/pages/wifi.tpl"),

	ROUTE_CGI("/wifi/scan.cgi", cgiWiFiScan),
	ROUTE_CGI("/wifi/connect.cgi", cgiWiFiConnect),
	ROUTE_CGI("/wifi/connstatus.cgi", cgiWiFiConnStatus),
	ROUTE_CGI("/wifi/setmode.cgi", cgiWiFiSetMode),

	// --- FS ---

	ROUTE_FS("*"), //Catch-all cgi function for the filesystem NOTE: unsafe, lets user read templates.

	ROUTE_END()
};


#if WIFI_PROTECT
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
static int FLASH_FN myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen)
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
#endif
