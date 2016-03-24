/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
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
#include "cgi.h"
#include "uptime.h"
#include "datalink.h"

// ---- demonstration of a multi-part substituion in a template ----

typedef struct {
	uint32_t count_remain;
} RandomNumberState;


//Template code for the counter on the index page.
int FLASH_FN tplMultipart(HttpdConnData *connData, char *token, void **arg)
{
	if (token == NULL) {
		if (*arg != NULL) {
			free(*arg);
			*arg = NULL; // mark as already freed
		}
		return HTTPD_CGI_DONE; // cleanup
	}

	if (os_strcmp(token, "numbers") == 0) {
		RandomNumberState *rns = *arg;
		char buff[20];

		// first call in this substitution
		if (rns == NULL) {
			//First call to this cgi. Open the file so we can read it.
			rns=(RandomNumberState *)malloc(sizeof(RandomNumberState));
			*arg=rns;

			// parse count
			uint32_t count = 1;
			int len = httpdFindArg(connData->getArgs, "count", buff, sizeof(buff));
			if (len==-1) {
				// no such get arg
			} else {
				count = (uint32_t)atoi(buff);
			}
			rns->count_remain = count;

			info("User wants %d numbers.", count);
		}

		// print the numbers
		for (int i = 0; i < 100; i++) {
			os_sprintf(buff, "<li>%lu\n", os_random());
			httpdSend(connData, buff, -1);

			if (--rns->count_remain == 0) {
				break;
			}
		}

		// We are done.
		if (rns->count_remain == 0) {
			free(rns);
			*arg = NULL; // mark as already freed
			return HTTPD_CGI_DONE;
		}

		return HTTPD_CGI_MORE;
	}

	return HTTPD_CGI_DONE;
}

/*
// better to put it in the fs...

int FLASH_FN cgiRandomNumbers(HttpdConnData *connData) {
	RandomNumberState *rns=connData->cgiData;
	char buff[128];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		if (rns != NULL) free(rns);
		return HTTPD_CGI_DONE;
	}

	if (rns == NULL) {
		//First call to this cgi. Open the file so we can read it.
		rns=(RandomNumberState *)malloc(sizeof(RandomNumberState));
		connData->cgiData=rns;

		// parse count
		uint32_t count = 1;
		int len = httpdFindArg(connData->getArgs, "count", buff, sizeof(buff));
		if (len==-1) {
			// no such get arg
		} else {
			count = (uint32_t)atoi(buff);
		}
		rns->count_remain = count;

		printf("User wants %d numbers.", count);

		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/html");
		httpdEndHeaders(connData);

		// start the page

		httpdSend(connData, "<!DOCTYPE html>"
							"<html>"
							"<head>"
							"	<title>Generated page.</title>"
							"	<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">"
							"</head>"
							"<body>"
							"<div id=\"main\">"
							"<h1>Random numbers:</h1>"
							"<ul>", -1);

		return HTTPD_CGI_MORE;
	}

	// send end of the page
	if (rns->count_remain == 0) {
		httpdSend(connData, "</ul></body></html>", -1);

		free(rns);
		return HTTPD_CGI_DONE;
	}


	// print chunk of data
	for (int i = 0; i < 100; i++) {
		os_sprintf(buff, "<li>%lu\n", os_random());
		httpdSend(connData, buff, -1);

		if (--rns->count_remain == 0) {
			break;
		}
	}

	return HTTPD_CGI_MORE;
}
*/
