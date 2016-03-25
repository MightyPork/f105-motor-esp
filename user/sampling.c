#include <esp8266.h>
#include <sbmp.h>

#include "datalink.h"
#include "sampling.h"
#include "timeout.h"

// The buffer is big enough for 256 data bytes - 4*64

// number of 32-bit vars in the chunk
#define CHUNK_LEN_32 64

// chunk size for bulk transfer - 64 floats
#define CHUNK_LEN (CHUNK_LEN_32*4)


static bool acquire_pending = false;
static uint16_t acquire_session;


static ETSTimer prSampleAbortTimer;

static void FLASH_FN prSampleAbortTimerCb(void *arg) {
	warn("Sampling aborted due to timeout.");
	acquire_pending = false;

	// free the data obj if not NULL
	sbmp_ep_free_listener_obj(dlnk_ep, acquire_session);
	// release the slot
	sbmp_ep_remove_listener(dlnk_ep, acquire_session);
}


typedef struct {
	uint32_t pos;
	uint32_t total;
} DataReadState;

static void FLASH_FN request_data_sesn_listener(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	bool suc = false;
	dbg("Received msg in session %d, dg type %d", dg->session, dg->type);

	DataReadState *state = *obj;

	// allocate the state struct
	if (state == NULL) {
		state = malloc(sizeof(DataReadState));
		*obj = state;
	}

	PayloadParser pp;
	switch (dg->type) {
		case DG_BULK_OFFER:// Data ready notification
			info("--- Data offered for bulk transfer ---");

			// data is ready to be read
			pp = pp_start(dg->payload, dg->length);

			state->pos = 0;
			state->total = pp_u32(&pp);

			dbg("Total bytes: %d", state->total);

			retry_until_timeout(10, sbmp_bulk_request(ep, state->pos, CHUNK_LEN, dg->session));
			break;

		case DG_BULK_DATA: // data received
			info("--- Received a chunk ---");

			// Process the received data
			pp = pp_start(dg->payload, dg->length);
			for (int i = 0; i < dg->length/4; i++) {
				float f = pp_float(&pp);
				dbg("Rx %.2f", f);
			}

			// and ask for more
			state->pos += dg->length;

			if (state->pos >= state->total) {
				dbg("Transfer is complete.");
				// transfer complete

				// make sure the peer has freed the buffer
				// (may be waiting for us if we wanted to re-read something)

				retry_until_timeout(10, sbmp_bulk_abort(ep, dg->session));
				goto cleanup;
			} else {
				// read next part
				retry_until_timeout(10, sbmp_bulk_request(ep, state->pos, CHUNK_LEN, dg->session));
			}
			break;

		case DG_BULK_ABORT: // Peer resets the readout
			// this is unlikely
			warn("Bulk transfer aborted by peer.");
			goto cleanup;
	}

	return;

cleanup:
	// free the obj
	free(state);
	// remove the listener
	sbmp_ep_remove_listener(ep, dg->session);
}


static bool FLASH_FN meas_request_data(uint16_t count)
{
	info("Requesting data capture - %d samples.", count);

	if (acquire_pending) {
		error("Acquire request already in progress.");
		return false;
	}

	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		error("Hsk status not SUCCESS, can't request data.");
		return false;
	}

	// start the abort timer - timeout
	os_timer_disarm(&prSampleAbortTimer);
	os_timer_setfn(&prSampleAbortTimer, prSampleAbortTimerCb, NULL);
	os_timer_arm(&prSampleAbortTimer, 5000, 0); // 5 seconds, no repeat


	// start a message
	uint16_t sesn;
	bool suc = sbmp_ep_start_message(dlnk_ep, DG_REQUEST_CAPTURE, sizeof(uint16_t), &sesn);
	if (!suc) goto fail;

	// register the session listener
	suc = sbmp_ep_add_listener(dlnk_ep, sesn, request_data_sesn_listener, NULL);
	if (!suc) goto fail;

	// request N values
	sbmp_ep_send_u16(dlnk_ep, count);

	dbg("Request sent, session nr %d", sesn);

	acquire_session = sesn;

	return true;

fail:
	os_timer_disarm(&prSampleAbortTimer);
	return false;
}


// ------ C G I ---------



int FLASH_FN cgiReadSamples(HttpdConnData *connData)
{
	char buff[128];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	uint16_t count = 1;
	int len = httpdFindArg(connData->getArgs, "n", buff, sizeof(buff));
	if (len != -1) {
		count = (uint16_t)atoi(buff);
	}

	dbg("User wants %d samples.", count);

	meas_request_data(count);

	httpdStartResponse(connData, 200);
	httpdHeader(connData, "Content-Type", "text/plain");
	httpdEndHeaders(connData);

	// body
	httpdSend(connData, "OK.", -1);

	return HTTPD_CGI_DONE;
}
