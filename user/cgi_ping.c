#include "cgi_ping.h"

int FLASH_FN cgiPing(HttpdConnData *connData)
{
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	httpdStartResponse(connData, 200);
	httpdEndHeaders(connData);

	httpdSend(connData, "pong\n", -1);

	return HTTPD_CGI_DONE;
}
