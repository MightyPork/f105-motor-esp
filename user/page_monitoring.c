#include <esp8266.h>
#include <httpd.h>
#include "page_monitoring.h"

/** "Monitoring" page - fill form fields */
int FLASH_FN tplMonitoring(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;

	char buf[20];

	if (token == NULL) return HTTPD_CGI_DONE;

	if (streq(token, "refStored")) {
		httpdSend(connData, true ? "OK" : "Not set!", -1); // fixme

	} else if (streq(token, "repEnableCheck")) {
		if (true) httpdSend(connData, "checked", -1); // fixme

	} else if (streq(token, "repInterval")) {
		sprintf(buf, "%d", 123); // fixme
		httpdSend(connData, buf, -1);

	} else if (streq(token, "repSvcCheckXv")) { // Xively
		if (true) httpdSend(connData, "checked", -1); // fixme

	} else if (streq(token, "repSvcCheckTs")) { // ThingSpeak
		if (true) httpdSend(connData, "checked", -1); // fixme

	} else if (streq(token, "repFeed")) {
		httpdSend(connData, "null", -1); // fixme

	} else if (streq(token, "repKey")) {
		httpdSend(connData, "null", -1); // fixme
	}

	return HTTPD_CGI_DONE;
}
