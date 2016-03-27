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
#include "sampling.h"
#include "serial.h"


// -------------------------------------------------------------------------------

// Read multiple samples from the ADC as JSON

typedef struct {
	uint16_t total_count;
	uint16_t done_count;
	bool success;
} tplReadSamplesJSON_state;


int FLASH_FN tplReadSamplesJSON(HttpdConnData *connData, char *token, void **arg)
{
	char buff20[20];

	tplReadSamplesJSON_state *st = *arg;

	if (token == NULL) {
		// end of template, cleanup
		if (st != NULL) free(st);
		return HTTPD_CGI_DONE; // cleanup
	}

	if (st == NULL) {
		// first call - allocate the struct
		st = malloc(sizeof(tplReadSamplesJSON_state));
		*arg = st;

		// check how many samples are requested
		uint16_t count = 1;
		int len = httpdFindArg(connData->getArgs, "n", buff20, sizeof(buff20));
		if (len != -1) count = (uint16_t)atoi(buff20);
		if (count > 4096) {
			warn("Requested %d samples, capping at 4096.", count);
			count = 4096;
		}
		st->total_count = count;
		st->done_count = 0;
		st->success = true; // success true by default

		// REQUEST THE DATA
		meas_request_data(st->total_count);
	}

	// the "success" field is after the data,
	// so if readout fails, success can be set to false.

	if (strcmp(token, "values") == 0) {

		// Wait for a chunk

		uint8_t *chunk = NULL;
		uint16_t chunk_len = 0;

		// 5 secs or 500 ms
		for (int i = 0; i < (st->done_count == 0 ? 5000*100: 500*100); i++) {
			uart_poll();
			if (meas_chunk_ready()) break;
			os_delay_us(10); // 1 ms
			system_soft_wdt_feed();
		}
		chunk = meas_get_chunk(&chunk_len);

		if (!chunk) {
			// abort, proceed to the next field.
			meas_close();
			st->success = false;
			return HTTPD_CGI_DONE;
		}

		PayloadParser pp = pp_start(chunk, chunk_len);

		// chunk of data...
		for (; pp.ptr < pp.len; st->done_count++) {
			// preceding comma if not the first number
			if (st->done_count > 0) {
				httpdSend(connData, ", ", 2);
			}

			uint32_t samp = pp_u32(&pp);

			// print the number
			os_sprintf(buff20, "%d", samp);
			httpdSend(connData, buff20, -1);
		}

		// wait for more in this substitution
		if (st->done_count < st->total_count) {
			meas_request_next_chunk();
			return HTTPD_CGI_MORE; // more numbers to come
		} else {
			// we're done
			meas_close();
			return HTTPD_CGI_DONE;
		}

	} else if (strcmp(token, "success") == 0) {
		// success status
		httpdSend(connData, (st->success ? "true" : "false"), -1);
	}

	return HTTPD_CGI_DONE;
}




// Example of multi-pass generation of a html file


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
							"   <title>Generated page.</title>"
							"   <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">"
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
