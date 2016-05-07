#include <esp8266.h>
#include <httpd.h>
#include "page_about.h"
#include "fw_version.h"
#include "sbmp.h"
#include "datalink.h"
#include "serial.h"

static bool stm_vers_loaded = false;
static char stm_vers_buf[10];

static void readVersionCB(SBMP_Endpoint *ep, SBMP_Datagram *dg, void** arg)
{
	(void)ep;
	(void)arg;

	if (dg->type != DG_SUCCESS) {
		error("Response to REQUEST_VERSION not SUCCESS.");
		return;
	}

	sprintf(stm_vers_buf, "%d.%d.%d", dg->payload[0], dg->payload[1], dg->payload[2]);
	stm_vers_loaded = true;
}


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

	} else if (streq(token, "vers_stm")) {

		if (stm_vers_loaded) {
			httpdSend(connData, stm_vers_buf, -1);
		} else {

			uint16_t sesn;
			sbmp_ep_send_message(dlnk_ep, DG_REQUEST_STM_VERSION, NULL, 0, &sesn, NULL);
			sbmp_ep_add_listener(dlnk_ep, sesn, readVersionCB, NULL);

			sprintf(stm_vers_buf, "???");

			// poll & wait for response
			const int timeout = 100;
			for (uint32_t i = 0; i < timeout * 100; i++) {
				uart_poll();

				if (stm_vers_loaded) {
					break;
				}

				os_delay_us(10);
				system_soft_wdt_feed(); // Feed the dog, or it'll bite.
			}

			httpdSend(connData, stm_vers_buf, -1); // send to view
		}
	}

	return HTTPD_CGI_DONE;
}
