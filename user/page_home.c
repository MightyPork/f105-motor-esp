#include <esp8266.h>
#include <httpd.h>
#include "fw_version.h"
#include "sbmp.h"
#include "datalink.h"
#include "serial.h"


/** "Home" page */
httpd_cgi_state FLASH_FN tplHome(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;
	(void)connData;

	if (token == NULL) return HTTPD_CGI_DONE;

//	if (streq(token, "vers_fw")) {
//		httpdSend(connData, FIRMWARE_VERSION, -1);
//	}

	return HTTPD_CGI_DONE;
}
