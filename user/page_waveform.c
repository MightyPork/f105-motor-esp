#include <esp8266.h>
#include <httpd.h>
#include "page_waveform.h"

#include "sampling.h"
#include "serial.h"
#include "payload_parser.h"

// -------------------------------------------------------------------------------

// Read multiple samples from the ADC as JSON

typedef struct {
	uint16_t total_count;
	uint16_t done_count;
	bool success;
} tplReadSamplesJSON_state;


int FLASH_FN tplWaveformJSON(HttpdConnData *connData, char *token, void **arg)
{
	char buff20[20];
	int len;

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
		len = httpdFindArg(connData->getArgs, "n", buff20, sizeof(buff20));
		if (len != -1) count = (uint16_t)atoi(buff20);
		if (count > 4096) {
			warn("Requested %d samples, capping at 4096.", count);
			count = 4096;
		}

		uint32_t freq = 4096;
		len = httpdFindArg(connData->getArgs, "fs", buff20, sizeof(buff20));
		if (len != -1) freq = (uint32_t)atoi(buff20);
		if (freq > 5000000) {
			warn("Requested fs %d Hz, capping at 5 MHz.", freq);
			freq = 5000000;
		}
		if (freq == 0) {
			error("Requested fs 0 Hz, using 1 Hz");
			freq = 1;
		}

		st->total_count = count;
		st->done_count = 0;
		st->success = true; // success true by default

		// REQUEST THE DATA
		meas_request_data(count, freq);
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



