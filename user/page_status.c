#include <esp8266.h>
#include <httpd.h>
#include "page_status.h"
#include "uptime.h"

#include "utils.h"


/** System Status page */
httpd_cgi_state FLASH_FN tplSystemStatus(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;

	struct station_config stconf;
	struct softap_config apconf;

	char buff[300];
	u8 mac[6];
	int rssi;

	// empty string if no token matches
	buff[0] = 0;

	if (token == NULL) return HTTPD_CGI_DONE;


//	{
//	  "uptime": "00:01:1200:01:12",
//	  "heap": 3391233912,
//	  "wifiMode": "ClientClient",
//	  "sta": null,
//	  "ap": {"SSID": "ESP_D58987", "pwd": "", "MAC": "1a:fe:34:d5:89:87", "chan": 1, "hidden": false, "auth": Open, }
//	}



	int opmode = wifi_get_opmode();
	bool is_sta = (opmode == STATION_MODE || opmode == STATIONAP_MODE);
	bool is_ap  = (opmode == SOFTAP_MODE || opmode == STATIONAP_MODE);

	if (streq(token, "uptime")) {
		// Uptime
		uptime_str(buff);

	} else if (streq(token, "heap")) {
		// Free heap
		sprintf(buff, "%u", system_get_free_heap_size());

	} else if (streq(token, "wifiMode")) {
		// WiFi mode
		strcpy(buff, opmode2str(opmode));

	} else if (streq(token, "staInfo")) {
		if (!is_sta) {
			strcpy(buff, "null");
		} else {
			// AP info
			wifi_station_get_config(&stconf);
			wifi_get_macaddr(STATION_IF, mac);
			rssi = wifi_station_get_rssi();

			sprintf(buff,
					"{"
					"\"SSID\": \"%s\", "
					"\"RSSI\": %d, "
					"\"RSSIperc\": %d, "
					"\"MAC\": \""MACSTR"\""
					"}",
					stconf.ssid,
					rssi,
					rssi2perc(rssi),
					MAC2STR(mac)
					);
		}

	} else if (streq(token, "apInfo")) {
		if (!is_ap) {
			strcpy(buff, "null");
		} else {
			wifi_softap_get_config(&apconf);
			wifi_get_macaddr(SOFTAP_IF, mac);

			// AP info
			sprintf(buff,
					"{"
					"\"SSID\": \"%s\", "
					"\"pwd\": \"%s\", "
					"\"MAC\": \""MACSTR"\", "
					"\"chan\": %d, "
					"\"hidden\": %s, "
					"\"auth\": \"%s\""
					"}",
					apconf.ssid,
					apconf.password,
					MAC2STR(mac),
					apconf.channel,
					apconf.ssid_hidden ? "true" : "false",
					auth2str(apconf.authmode)
					);
		}

	/*} else if (streq(token, "staSSID")) {
		// Station SSID (if in station mode)
		if (!is_sta) {
			strcpy(buff, "N/A");
		} else {
			strcpy(buff, (char*)stconf.ssid);
		}

	} else if (streq(token, "staRSSI")) {
		// Signal strength if in Station mode
		if (!is_sta) {
			strcpy(buff, "0");
		} else {
			rssi = wifi_station_get_rssi();
			sprintf(buff, "%d", rssi);
		}

	} else if (streq(token, "staRSSIperc")) {
		// Signal strength if in Station mode
		if (!is_sta) {
			strcpy(buff, "0");
		} else {
			rssi = wifi_station_get_rssi();
			sprintf(buff, "%d", rssi2perc(rssi));
		}

	} else if (streq(token, "staMAC")) {
		// Station MAC addr
		wifi_get_macaddr(STATION_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (streq(token, "apMAC")) {
		// SoftAP MAC addr
		wifi_get_macaddr(SOFTAP_IF, mac);
		sprintf(buff, MACSTR, MAC2STR(mac));

	} else if (streq(token, "chipID")) {
		// Chip serial number
		sprintf(buff, "%08x", system_get_chip_id());*/
	}

	httpdSend(connData, buff, -1);

	return HTTPD_CGI_DONE;
}

/*
  "sta": {
	"SSID": "%staSSID%",
	"RSSI": %staRSSI%,
	"RSSIperc": %staRSSIperc%,
	"MAC": "%staMAC%"
  },
  "ap": {
	"MAC": "%apMAC%"
  },
*/
