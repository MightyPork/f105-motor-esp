/*
Cgi/template routines for the /wifi url.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include "cgiwifi.h"

#include "utils.h" // ADDED

//Enable this to disallow any changes in AP settings
//#define DEMO_MODE



static ETSTimer resetTmr;

void FLASH_FN resetTmrCb(void *arg)
{
	system_restart();
}

static void FLASH_FN reset_later(int ms)
{
	os_timer_disarm(&resetTmr);
	os_timer_setfn(&resetTmr, resetTmrCb, NULL);
	os_timer_arm(&resetTmr, ms, false);
}



//WiFi access point data
typedef struct {
	char ssid[32];
	char bssid[8];
	int channel;
	sint8 rssi;
	uint8 enc;
} ApData;

//Scan result
typedef struct {
	char scanInProgress; //if 1, don't access the underlying stuff from the webpage.
	ApData **apData;
	int noAps;
} ScanResultData;

//Static scan status storage.
static ScanResultData cgiWifiAps;

#define CONNTRY_IDLE 0
#define CONNTRY_WORKING 1
#define CONNTRY_SUCCESS 2
#define CONNTRY_FAIL 3
//Connection result var
static int connTryStatus=CONNTRY_IDLE;
static os_timer_t resetTimer;

//Callback the code calls when a wlan ap scan is done. Basically stores the result in
//the cgiWifiAps struct.
void ICACHE_FLASH_ATTR wifiScanDoneCb(void *arg, STATUS status) {
	int n;
	struct bss_info *bss_link = (struct bss_info *)arg;
	dbg("wifiScanDoneCb %d\n", status);
	if (status!=OK) {
		cgiWifiAps.scanInProgress=0;
		return;
	}

	//Clear prev ap data if needed.
	if (cgiWifiAps.apData!=NULL) {
		for (n=0; n<cgiWifiAps.noAps; n++) free(cgiWifiAps.apData[n]);
		free(cgiWifiAps.apData);
	}

	//Count amount of access points found.
	n=0;
	while (bss_link != NULL) {
		bss_link = bss_link->next.stqe_next;
		n++;
	}
	//Allocate memory for access point data
	cgiWifiAps.apData=(ApData **)malloc(sizeof(ApData *)*n);
	cgiWifiAps.noAps=n;
	info("Scan done: found %d APs", n);

	//Copy access point data to the static struct
	n=0;
	bss_link = (struct bss_info *)arg;
	while (bss_link != NULL) {
		if (n>=cgiWifiAps.noAps) {
			//This means the bss_link changed under our nose. Shouldn't happen!
			//Break because otherwise we will write in unallocated memory.
			warn("Huh? I have more than the allocated %d aps!", cgiWifiAps.noAps);
			break;
		}
		//Save the ap data.
		cgiWifiAps.apData[n]=(ApData *)malloc(sizeof(ApData));
		cgiWifiAps.apData[n]->rssi=bss_link->rssi;
		cgiWifiAps.apData[n]->channel=bss_link->channel;
		cgiWifiAps.apData[n]->enc=bss_link->authmode;
		strncpy(cgiWifiAps.apData[n]->ssid, (char*)bss_link->ssid, 32);
		strncpy(cgiWifiAps.apData[n]->bssid, (char*)bss_link->bssid, 6);

		bss_link = bss_link->next.stqe_next;
		n++;
	}
	//We're done.
	cgiWifiAps.scanInProgress=0;
}


//Routine to start a WiFi access point scan.
static void ICACHE_FLASH_ATTR wifiStartScan() {
//	int x;
	if (cgiWifiAps.scanInProgress) return;
	cgiWifiAps.scanInProgress=1;
	wifi_station_scan(NULL, wifiScanDoneCb);
}

//This CGI is called from the bit of AJAX-code in wifi.tpl. It will initiate a
//scan for access points and if available will return the result of an earlier scan.
//The result is embedded in a bit of JSON parsed by the javascript in wifi.tpl.
httpd_cgi_state ICACHE_FLASH_ATTR cgiWiFiScan(HttpdConnData *connData) {
	int pos=(int)connData->cgiData;
	int len;
	char buff[256];

	if (!cgiWifiAps.scanInProgress && pos!=0) {
		//Fill in json code for an access point
		if (pos-1<cgiWifiAps.noAps) {
			int rssi = cgiWifiAps.apData[pos-1]->rssi;

			len=sprintf(buff, "{\"essid\": \"%s\", \"bssid\": \""MACSTR"\", \"rssi\": %d, \"rssi_perc\": %d, \"enc\": %d, \"channel\": %d}%s",
					cgiWifiAps.apData[pos-1]->ssid,
					MAC2STR(cgiWifiAps.apData[pos-1]->bssid),
					rssi,
					rssi2perc(rssi),
					cgiWifiAps.apData[pos-1]->enc,
					cgiWifiAps.apData[pos-1]->channel,
					(pos-1==cgiWifiAps.noAps-1) ? "" : ","); //<-terminator

			httpdSend(connData, buff, len);
		}
		pos++;
		if ((pos-1)>=cgiWifiAps.noAps) {
			len=sprintf(buff, "]}}"); // terminate the whole object
			httpdSend(connData, buff, len);
			//Also start a new scan.
			wifiStartScan();
			return HTTPD_CGI_DONE;
		} else {
			connData->cgiData=(void*)pos;
			return HTTPD_CGI_MORE;
		}
	}

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "application/json");
	httpdEndHeaders(connData);

	if (cgiWifiAps.scanInProgress==1) {
		//We're still scanning. Tell Javascript code that.
		len=sprintf(buff, "{\"result\":{\"inProgress\":1}}");
		httpdSend(connData, buff, len);
		return HTTPD_CGI_DONE;
	} else {
		//We have a scan result. Pass it on.
		len=sprintf(buff, "{\"result\":{\"inProgress\":0,\"APs\":[");
		httpdSend(connData, buff, len);
		if (cgiWifiAps.apData==NULL) cgiWifiAps.noAps=0;
		connData->cgiData=(void *)1;
		return HTTPD_CGI_MORE;
	}
}

//Temp store for new ap info.
static struct station_config stconf;

//This routine is ran some time after a connection attempt to an access point. If
//the connect succeeds, this gets the module in STA-only mode.
static void ICACHE_FLASH_ATTR staResetTimerCb(void *arg) {
	int x=wifi_station_get_connect_status();
	if (x==STATION_GOT_IP) {
		//Go to STA mode. This needs a reset, so do that.
		info("Got IP. Going into STA mode.");
		wifi_set_opmode(1);
		system_restart();
	} else {
		connTryStatus=CONNTRY_FAIL;
		error("Connect fail. Not going into STA-only mode.");
		//Maybe also pass this through on the webpage?
	}
}



//Actually connect to a station. This routine is timed because I had problems
//with immediate connections earlier. It probably was something else that caused it,
//but I can't be arsed to put the code back :P
static void ICACHE_FLASH_ATTR reassTimerCb(void *arg) {
	int x;
	dbg("Try to connect to AP...");
	wifi_station_disconnect();
	wifi_station_set_config(&stconf);
	wifi_station_connect();
	x=wifi_get_opmode();
	connTryStatus=CONNTRY_WORKING;
	if (x != STATION_MODE) {
		//Schedule disconnect/connect
		os_timer_disarm(&resetTimer);
		os_timer_setfn(&resetTimer, staResetTimerCb, NULL);
		os_timer_arm(&resetTimer, 15000, 0); //time out after 15 secs of trying to connect
	}
}


//This cgi uses the routines above to connect to a specific access point with the
//given ESSID using the given password.
httpd_cgi_state ICACHE_FLASH_ATTR cgiWiFiConnect(HttpdConnData *connData) {
	char essid[128];
	char passwd[128];
	static os_timer_t reassTimer;

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	httpdFindArg(connData->post->buff, "essid", essid, sizeof(essid));
	httpdFindArg(connData->post->buff, "passwd", passwd, sizeof(passwd));

	strncpy((char*)stconf.ssid, essid, 32);
	strncpy((char*)stconf.password, passwd, 64);
	info("Try to connect to AP %s pw %s", essid, passwd);

	//Schedule disconnect/connect
	os_timer_disarm(&reassTimer);
	os_timer_setfn(&reassTimer, reassTimerCb, NULL);
//Set to 0 if you want to disable the actual reconnecting bit
#ifdef DEMO_MODE
	httpdRedirect(connData, "/wifi");
#else
	os_timer_arm(&reassTimer, 500, 0);
	httpdRedirect(connData, "/wifi/connecting.html"); // TODO custom page. Also diff. b/w sta & ap.
#endif
	return HTTPD_CGI_DONE;
}

//This cgi uses the routines above to connect to a specific access point with the
//given ESSID using the given password.
httpd_cgi_state ICACHE_FLASH_ATTR cgiWiFiSetMode(HttpdConnData *connData) {
	int len;
	char buff[64];

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->getArgs, "mode", buff, sizeof(buff));
	if (len!=0) {
		info("cgiWifiSetMode: %s", buff);
#ifndef DEMO_MODE
		wifi_set_opmode(atoi(buff));

		reset_later(100);

		//system_restart(); // FIXME we should do this in a timer task, so the browser gets a response.
#endif
	}
	httpdRedirect(connData, "/wifi");
	return HTTPD_CGI_DONE;
}

httpd_cgi_state ICACHE_FLASH_ATTR cgiWiFiConnStatus(HttpdConnData *connData) {
	char buff[256];
	int len;
	struct ip_info info;
	int st=wifi_station_get_connect_status();
	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "application/json");
	httpdEndHeaders(connData);
	if (connTryStatus==CONNTRY_IDLE) {
		len=sprintf(buff, "{\"status\":\"idle\"}");
	} else if (connTryStatus==CONNTRY_WORKING || connTryStatus==CONNTRY_SUCCESS) {
		if (st==STATION_GOT_IP) {
			wifi_get_ip_info(0, &info);
			len=sprintf(buff, "{\"status\":\"success\",\"ip\":\"%d.%d.%d.%d\"}",
				(info.ip.addr>>0)&0xff, (info.ip.addr>>8)&0xff,
				(info.ip.addr>>16)&0xff, (info.ip.addr>>24)&0xff);
			//Reset into AP-only mode sooner.
			os_timer_disarm(&resetTimer);
			os_timer_setfn(&resetTimer, staResetTimerCb, NULL);
			os_timer_arm(&resetTimer, 1000, 0);
		} else {
			len=sprintf(buff, "{\"status\":\"working\"}");
		}
	} else {
		len=sprintf(buff, "{\"status\":\"fail\"}\n");
	}

	httpdSend(connData, buff, len);
	return HTTPD_CGI_DONE;
}

//Template code for the WLAN page.
httpd_cgi_state ICACHE_FLASH_ATTR tplWlan(HttpdConnData *connData, char *token, void **arg) {
	char buff[256];
	WIFI_MODE x;
	static struct station_config stconf;
	if (token==NULL) return HTTPD_CGI_DONE;
	wifi_station_get_config(&stconf);

	strcpy(buff, "Unknown");
	if (strcmp(token, "WiFiMode")==0) {
		x=wifi_get_opmode();
		strcpy(buff, opmode2str(x));
	} else if (strcmp(token, "currSsid")==0) {
		strcpy(buff, (char*)stconf.ssid);
//	} else if (strcmp(token, "WiFiPasswd")==0) { // don't expose password
//		strcpy(buff, (char*)stconf.password);
	} else if (strcmp(token, "WiFiapwarn")==0) {
		x=wifi_get_opmode();
		if (x==SOFTAP_MODE) {
			strcpy(buff, "<b>Can't scan in this mode.</b> Click <a href=\"/wifi/setmode?mode=3\">here</a> to go to STA+AP mode.");
		} else {
			strcpy(buff, "Click <a href=\"/wifi/setmode?mode=2\">here</a> to go to stand-alone AP mode.");
		}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

