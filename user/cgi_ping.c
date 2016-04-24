#include "cgi_ping.h"

httpd_cgi_state FLASH_FN cgiPing(HttpdConnData *connData)
{
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "text/plain");
	httpdEndHeaders(connData);

	httpdSend(connData, "pong\n", -1);

	return HTTPD_CGI_DONE;
}
