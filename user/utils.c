#include <esp8266.h>
#include "utils.h"

int FLASH_FN rssi2perc(int rssi)
{
	int r;

	// convert to percentage
	if (rssi > -50)
		r = 100;
	else if (rssi < -100)
		r = 0;
	else
		r = 2 * (rssi + 100); // approx.

	return r;
}

const FLASH_FN char *auth2str(AUTH_MODE auth)
{
	switch (auth) {
		case AUTH_OPEN: return "Open";
		case AUTH_WEP: return "WEP";
		case AUTH_WPA_PSK: return "WPA";
		case AUTH_WPA2_PSK: return "WPA2";
		case AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
		default:
			return "Unknown";
	}
}

const FLASH_FN char *opmode2str(WIFI_MODE opmode)
{
	switch (opmode) {
		case NULL_MODE: return "Disabled";
		case STATION_MODE: return "Client";
		case SOFTAP_MODE: return "SoftAP";
		case STATIONAP_MODE: return "STA+AP";
		default:
			return "Unknown";
	}
}
