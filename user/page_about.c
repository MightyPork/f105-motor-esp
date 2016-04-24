#include <esp8266.h>
#include <httpd.h>
#include "page_about.h"
#include "fw_version.h"
#include "sbmp.h"

/** "About" page */
httpd_cgi_state FLASH_FN tplAbout(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;

	if (token == NULL) return HTTPD_CGI_DONE;

	if (streq(token, "vers_fw")) {
		httpdSend(connData, FIRMWARE_VERSION, -1);

	} else if (streq(token, "date")) {
		httpdSend(connData, __DATE__, -1);

	} else if (streq(token, "time")) {
		httpdSend(connData, __TIME__, -1);

	} else if (streq(token, "vers_httpd")) {
		httpdSend(connData, HTTPDVER, -1);

	} else if (streq(token, "vers_sbmp")) {
		httpdSend(connData, SBMP_VER, -1);

	} else if (streq(token, "vers_sdk")) {
		httpdSend(connData, STR(ESP_SDK_VERSION), -1);
	}

	return HTTPD_CGI_DONE;
}
