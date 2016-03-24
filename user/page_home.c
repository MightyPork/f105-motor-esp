#include <esp8266.h>
#include <httpd.h>
#include "page_home.h"
#include "uptime.h"


/** System Status page */
int FLASH_FN tplHome(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;

	struct station_config stconf;
	char buff[128];
	u8 mac[6];

	// empty string if no token matches
	buff[0] = 0;

	if (token == NULL) return HTTPD_CGI_DONE;

	if (strcmp(token, "uptime") == 0) {
		// Uptime
		uptime_str(buff);

	} else if (strcmp(token, "heap") == 0) {
		// Free heap
		sprintf(buff, "%u", system_get_free_heap_size());

	} else if (strcmp(token, "wifiMode") == 0) {
		// WiFi mode
		switch (wifi_get_opmode()) {
			case STATION_MODE:   strcpy(buff, "Client"); break;
			case SOFTAP_MODE:    strcpy(buff, "SoftAP"); break;
			case STATIONAP_MODE: strcpy(buff, "STA+AP"); break;
			default: strcpy(buff, "Unknown");
		}

	} else if (strcmp(token, "staSSID") == 0) {
		// Station SSID (if in station mode)
		int opmode = wifi_get_opmode();
		if (opmode != STATION_MODE && opmode != STATIONAP_MODE) {
			strcpy(buff, "N/A"); // no SSID in AP-only mode
		} else {
			wifi_station_get_config(&stconf);
			strcpy(buff, (char*)stconf.ssid);
		}

	} else if (strcmp(token, "staRSSI") == 0) {
		// Signal strength if in Station mode
		int rssi = wifi_station_get_rssi();
		sprintf(buff, "%d", rssi);

	} else if (strcmp(token, "staMAC") == 0) {
		// Station MAC addr
		wifi_get_macaddr(STATION_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (strcmp(token, "apMAC") == 0) {
		// SoftAP MAC addr
		wifi_get_macaddr(SOFTAP_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (strcmp(token, "chipID") == 0) {
		// Chip serial number
		sprintf(buff, "%08x", system_get_chip_id());
	}

	httpdSend(connData, buff, -1);

	return HTTPD_CGI_DONE;
}
