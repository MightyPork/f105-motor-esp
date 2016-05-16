#include <esp8266.h>
#include <httpd.h>
#include "fw_version.h"
#include "sbmp.h"
#include "datalink.h"
#include "serial.h"


httpd_cgi_state FLASH_FN cgiMotorStart(HttpdConnData *connData)
{
	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	info("Motor start!");

	int suc;
	bool fwd = true;
	uint32_t speed = 6144*2;
	char buf[64];

	suc = httpdFindArg(connData->getArgs, "dir", buf, 64);
	if (suc != -1) {
		fwd = (buf[0] == 'f'); // dir=f or dir=r
	}

	suc = httpdFindArg(connData->getArgs, "speed", buf, 64);
	if (suc != -1) {
		speed = (uint32_t)atoi(buf);
	}

	dbg("Speed = %d, rev = %d", speed, fwd);

	sbmp_ep_start_message(dlnk_ep, DG_MOTOR_START, 5, NULL);
	sbmp_ep_send_u8(dlnk_ep, fwd ? 1 : 0);
	sbmp_ep_send_u32(dlnk_ep, speed);


	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "text/json");
	httpdEndHeaders(connData);

	httpdSend(connData, "{\"success\":true}\n", -1);

	return HTTPD_CGI_DONE;
}


httpd_cgi_state FLASH_FN cgiMotorStop(HttpdConnData *connData)
{
	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	info("Motor stop!");

	sbmp_ep_start_message(dlnk_ep, DG_MOTOR_STOP, 0, NULL);


	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "text/json");
	httpdEndHeaders(connData);
	httpdSend(connData, "{\"success\":true}\n", -1);
	return HTTPD_CGI_DONE;
}


/** "Home" page */
httpd_cgi_state FLASH_FN tplHome(HttpdConnData *connData, char *token, void **arg)
{
	// arg is unused
	(void)arg;
	(void)connData;

	if (token == NULL) return HTTPD_CGI_DONE;

//  if (streq(token, "vers_fw")) {
//      httpdSend(connData, FIRMWARE_VERSION, -1);
//  }

	return HTTPD_CGI_DONE;
}
