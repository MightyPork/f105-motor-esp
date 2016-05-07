#include <esp8266.h>
#include <httpd.h>
#include "wificontrol.h"
#include <sbmp.h>
#include "datalink.h"
#include "reporting.h"

// --- AP ---

void FLASH_FN wificontrol_setmode_ap(void)
{
	wifi_station_disconnect(); // disconnect
	wifi_set_opmode(SOFTAP_MODE); // enter AP mode

	warn("SBMP request to switch to SoftAP mode. Restarting...\n");

	system_restart(); // TODO check if restart is needed
}

// --- WPS ---

static bool wps_in_progress = false;

static ETSTimer wps_abort_timer;

/** Timer CB - abort WPS */
static void FLASH_FN abort_wps_cb(void *arg)
{
	(void)arg;
	error("WPS timeout, stopped.");
	wifi_wps_disable();
	wps_in_progress = false;
}

static void FLASH_FN my_wps_cb(int status)
{
	dbg("WPS callback!");
	wps_in_progress = false;

	switch (status) {
		case WPS_CB_ST_SUCCESS:
			info("WPS success!");
			wifi_wps_disable();
			wifi_station_connect();
			break;

		default:
			error("WPS failed.");
			break;
	}

	// TODO send status update to STM32
}

void FLASH_FN wificontrol_start_wps(void)
{
	info("Starting WPS...");

	// Make sure station is enabled
	if (wifi_get_opmode() == SOFTAP_MODE) {
		wifi_set_opmode(STATIONAP_MODE);
	}

	// Disconnect if connected to any station
	wifi_station_disconnect();

	// enable WPS
	wifi_set_wps_cb(my_wps_cb);
	wifi_wps_enable(WPS_TYPE_PBC);

	os_timer_disarm(&wps_abort_timer);
	os_timer_setfn(&wps_abort_timer, abort_wps_cb, NULL);
	os_timer_arm(&wps_abort_timer, 15000, false); // 15 seconds

	wps_in_progress = true;
}

// --- Status update ---

static ETSTimer wifistatus_timer;

/** Send wifi status update */
static void FLASH_FN wifistatus_cb(void *arg)
{
	(void)arg;

	WIFI_MODE opmode = wifi_get_opmode();

	STATION_STATUS status = STATION_IDLE;
	if (opmode != SOFTAP_MODE) {
		status = wifi_station_get_connect_status();
	}

	if (sbmp_ep_start_message(dlnk_ep, DG_WIFI_STATUS, 4, NULL)) {
		sbmp_ep_send_u8(dlnk_ep, opmode); // 1-Client, 2-SoftAP, 3-STA+AP
		sbmp_ep_send_u8(dlnk_ep, status == STATION_GOT_IP); // Station connected
		sbmp_ep_send_u8(dlnk_ep, wps_in_progress); // 1 = WPS is in progress
		sbmp_ep_send_u8(dlnk_ep, rpt_conf.enabled); // Reporting is enabled
	}
}

void FLASH_FN wificontrol_init(void)
{
	os_timer_disarm(&wifistatus_timer);
	os_timer_setfn(&wifistatus_timer, wifistatus_cb, NULL);
	os_timer_arm(&wifistatus_timer, 1000, true);
}
