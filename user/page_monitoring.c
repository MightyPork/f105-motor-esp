#include <esp8266.h>
#include <httpd.h>
#include "page_monitoring.h"
#include "reporting.h"
#include "ftoa.h"


httpd_cgi_state FLASH_FN cgiMonCompare(HttpdConnData *connData)
{
	if (connData->conn == NULL) return HTTPD_CGI_DONE;

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "application/json");
	httpdEndHeaders(connData);

	// this is semi-async (waits for completion)
	bool suc = capture_and_report(false);

	char buf[100];

	if (suc && rpt_result.ready) {
		// success
		char *bb = buf;
		bb += sprintf(bb, "{\"success\": true, \"deviation\": ");
		bb += my_ftoa(bb,rpt_result.deviation, 2);
		bb += sprintf(bb, ", \"rms\": ");
		bb += my_ftoa(bb,rpt_result.i_rms, 2);
		bb += sprintf(bb, "}");

		httpdSend(connData, buf, -1);
	} else {
		httpdSend(connData, "{\"success\": false}", -1);
	}

	return HTTPD_CGI_DONE;
}

/** This is an automated poll for current state, to update the display (measured by reporting func) */
httpd_cgi_state FLASH_FN cgiMonStatus(HttpdConnData *connData)
{
	if (connData->conn == NULL) return HTTPD_CGI_DONE;

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "application/json");
	httpdEndHeaders(connData);

	char buf[100];

	if (rpt_result.ready) {
		// success
		char *bb = buf;
		bb += sprintf(bb, "{\"success\": true, \"deviation\": ");
		bb += my_ftoa(bb,rpt_result.deviation, 2);
		bb += sprintf(bb, ", \"rms\": ");
		bb += my_ftoa(bb,rpt_result.i_rms, 2);
		bb += sprintf(bb, "}");
		httpdSend(connData, buf, -1);
	} else {
		httpdSend(connData, "{\"success\": false}", -1);
	}

	return HTTPD_CGI_DONE;
}


httpd_cgi_state FLASH_FN cgiMonSetRef(HttpdConnData *connData)
{
	if (connData->conn == NULL) return HTTPD_CGI_DONE;

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "application/json");
	httpdEndHeaders(connData);

	// this is semi-async (waits for completion)
	bool suc = capture_reporting_reference();
	httpdSend(connData, suc ? "{\"success\": true}" : "{\"success\": false}", -1);

	return HTTPD_CGI_DONE;
}


httpd_cgi_state FLASH_FN cgiMonitoringCfg(HttpdConnData *connData)
{
	if (connData->conn == NULL) return HTTPD_CGI_DONE;

	// TODO
	HttpdPostData *post = connData->post;
	if (post != NULL) {
		char buf[64];
		int blen;

		// enabled=1
		blen = httpdFindArg(post->buff, "enabled", buf, 64);
		if (blen == -1) {
			// wasn't found
			rpt_conf.enabled = false;
		} else {
			rpt_conf.enabled = (buf[0] == '1');
		}

		// interval=secs
		blen = httpdFindArg(post->buff, "interval", buf, 64);
		if (blen != -1) {
			rpt_conf.interval = (uint32_t)atoi(buf);
		}

		// service=xv or ts
		blen = httpdFindArg(post->buff, "service", buf, 64);
		if (blen != -1) {
			rpt_conf.service = (buf[0] == 'x' ? RPT_XIVELY: RPT_THINGSPEAK);
		}

		// feed
		blen = httpdFindArg(post->buff, "feed", buf, 64);
		if (blen != -1) {
			strcpy(rpt_conf.feed, buf);
		}

		// key
		blen = httpdFindArg(post->buff, "key", buf, 64);
		if (blen != -1) {
			strcpy(rpt_conf.key, buf);
		}

		// Save & Apply
		reporting_cfg_save();
	}

	httpdRedirect(connData, "/monitoring");
	return HTTPD_CGI_DONE;
}


/** "Monitoring" page - fill form fields */
httpd_cgi_state FLASH_FN tplMonitoring(HttpdConnData *connData, char *token, void **arg)
{
	(void)arg;

	char buf[20];

	if (token == NULL) return HTTPD_CGI_DONE;

	if (streq(token, "refStored")) {
		httpdSend(connData, true ? "OK" : "Not set!", -1); // fixme

	} else if (streq(token, "curDeviation")) {
		// current deviation
		if (rpt_result.ready) {
			my_ftoa(buf, rpt_result.deviation, 2);
		} else {
			sprintf(buf, "--");
		}
		httpdSend(connData, buf, -1);

	} else if (streq(token, "curRMS")) {
		// current deviation
		if (rpt_result.ready) {
			my_ftoa(buf, rpt_result.i_rms, 2);
		} else {
			sprintf(buf, "--");
		}
		httpdSend(connData, buf, -1);

	} else if (streq(token, "rep_en")) {
		if (rpt_conf.enabled) httpdSend(connData, "checked", -1);

	} else if (streq(token, "repInterval")) { // interval in seconds
		sprintf(buf, "%d", rpt_conf.interval);
		httpdSend(connData, buf, -1);

	} else if (streq(token, "svc_xv")) { // Xively checkbox
		if (rpt_conf.service == RPT_XIVELY) {
			httpdSend(connData, "checked", -1);
		}

	} else if (streq(token, "svc_ts")) { // ThingSpeak checkbox
		if (rpt_conf.service == RPT_THINGSPEAK) {
			httpdSend(connData, "checked", -1);
		}

	} else if (streq(token, "repFeed")) { // reporting feed ID
		httpdSend(connData, rpt_conf.feed, -1);

	} else if (streq(token, "repKey")) { // reporting key
		httpdSend(connData, rpt_conf.key, -1);
	}

	return HTTPD_CGI_DONE;
}
