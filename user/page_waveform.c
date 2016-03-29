#include <esp8266.h>
#include <httpd.h>
#include "page_waveform.h"

#include "ftoa.h"
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


static int FLASH_FN tplSamplesJSON(MEAS_FORMAT fmt, HttpdConnData *connData, char *token, void **arg);


int FLASH_FN tplWaveformJSON(HttpdConnData *connData, char *token, void **arg)
{
	return tplSamplesJSON(RAW, connData, token, arg);
}


int FLASH_FN tplFourierJSON(HttpdConnData *connData, char *token, void **arg)
{
	return tplSamplesJSON(FFT, connData, token, arg);
}


static int FLASH_FN tplSamplesJSON(MEAS_FORMAT fmt, HttpdConnData *connData, char *token, void **arg)
{
	char buff[128];
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
		len = httpdFindArg(connData->getArgs, "n", buff, sizeof(buff));
		if (len != -1) count = (uint16_t)atoi(buff);
		if (count == 0) {
			error("Count == 0");
			st->success = false;
			return HTTPD_CGI_DONE;
		}

		uint32_t freq = 0;
		len = httpdFindArg(connData->getArgs, "fs", buff, sizeof(buff));
		if (len != -1) freq = (uint32_t)atoi(buff);
		if (freq == 0) {
			error("Freq == 0");
			st->success = false;
			return HTTPD_CGI_DONE;
		}

		st->total_count = count;
		st->done_count = 0;

		// --- REQUEST THE DATA ---
		// This actually asks the STM32 over SBMP to start the ADC DMA,
		// and we'll wait for the data to arrive.
		st->success = meas_request_data(fmt, count, freq);
		if (!st->success) {
			error("Failed to start sampling");
		}
	}

	// the "success" field is after the data,
	// so if readout fails, success can be set to false.

	if (strcmp(token, "values") == 0) {
		if (!st->success) {
			// failed to start sampling
			return HTTPD_CGI_DONE;
		}

		// Wait for a chunk

		uint8_t *chunk = NULL;
		uint16_t chunk_len = 0;

		// 10 secs or 100 ms - longer wait for intial data.
		for (int i = 0; i < (st->done_count == 0 ? SAMPLING_TMEO*100: SAMP_READOUT_TMEO*100); i++) {
			uart_poll();
			if (meas_chunk_ready() || meas_is_closed()) break; // We have some data! --- or transaction aborted by peer :(
			os_delay_us(10); // 1 ms
			system_soft_wdt_feed(); // Feed the dog, or it'll bite.
		}
		chunk = meas_get_chunk(&chunk_len);

		if (chunk == NULL) {
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

			float samp = pp_float(&pp);
			my_ftoa(buff, samp, 3);
			httpdSend(connData, buff, -1);
		}

		// wait for more data in this % tag
		if (!meas_is_last_chunk()) {
			meas_request_next_chunk();
			return HTTPD_CGI_MORE; // more numbers to come
		} else {
			// we're done
			meas_close();
			return HTTPD_CGI_DONE;
		}

	} else if (strcmp(token, "stats") == 0) {

		// the STATS json block
		if (!st->success) {
			httpdSend(connData, "null", 4);
		} else {
			// no %f in sprintf :(

			MeasStats *stats = meas_get_stats();
			httpdSend(connData, "{", 1);

			sprintf(buff, "\"count\": %d, ", stats->count);
			httpdSend(connData, buff, -1);

			httpdSend(connData, "\"freq\": ", -1);
			my_ftoa(buff, stats->freq, 3);
			httpdSend(connData, buff, -1);

			httpdSend(connData, ", \"min\": ", -1);
			my_ftoa(buff, stats->min, 3);
			httpdSend(connData, buff, -1);

			httpdSend(connData, ", \"max\": ", -1);
			my_ftoa(buff, stats->max, 3);
			httpdSend(connData, buff, -1);

			httpdSend(connData, ", \"rms\": ", -1);
			my_ftoa(buff, stats->rms, 3);
			httpdSend(connData, buff, -1);

			if (fmt == FFT) {
				// ... maybe something special for fft ...
			}

			sprintf(buff, ", \"format\": \"%s\"", fmt == RAW ? "RAW" : fmt == FFT ? "FFT" : "UNKNOWN");
			httpdSend(connData, buff, -1);

			httpdSend(connData, "}", 1);
		}

	} else if (strcmp(token, "success") == 0) {
		// success status
		httpdSend(connData, (st->success ? "true" : "false"), -1);
	}

	return HTTPD_CGI_DONE;
}



