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


static volatile bool acquire_pending = false;
static volatile uint16_t acquire_session;


static ETSTimer prSampleAbortTimer;

static void FLASH_FN prSampleAbortTimerCb(void *arg)
{
	warn("Sampling aborted due to timeout.");

	// try to abort the readout
	sbmp_bulk_abort(dlnk_ep, acquire_session);

	// free the data obj if not NULL
	sbmp_ep_free_listener_obj(dlnk_ep, acquire_session);
	// release the slot
	sbmp_ep_remove_listener(dlnk_ep, acquire_session);
	acquire_pending = false;
}


static void setReadoutTmeoTimer(int ms)
{
	os_timer_disarm(&prSampleAbortTimer);
	os_timer_setfn(&prSampleAbortTimer, prSampleAbortTimerCb, NULL);
	os_timer_arm(&prSampleAbortTimer, ms, 0);
}

static void stopReadoutTmeoTimer(void)
{
	os_timer_disarm(&prSampleAbortTimer);
}


typedef struct {
	uint32_t pos;
	uint32_t total;
} DataReadState;

static void FLASH_FN request_data_sesn_listener(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	bool suc = false;
	dbg("Received msg in session %d, dg type %d", dg->session, dg->type);

	DataReadState *readState = *obj;

	// allocate the state struct
	if (readState == NULL) {
		readState = malloc(sizeof(DataReadState));
		*obj = readState;
	}

	PayloadParser pp;
	switch (dg->type) {
		case DG_BULK_OFFER:// Data ready notification
			info("--- Data offered for bulk transfer ---");
			setReadoutTmeoTimer(1000);

			// data is ready to be read
			pp = pp_start(dg->payload, dg->length);

			readState->pos = 0;
			readState->total = pp_u32(&pp);

			dbg("Total bytes: %d", readState->total);

			// we choose to request the data immediately

			retry_TO(100, sbmp_bulk_request(ep, readState->pos, CHUNK_LEN, dg->session));
			break;

		case DG_BULK_DATA: // data received
			info("--- Received a chunk, length %d ---", dg->length);
			setReadoutTmeoTimer(1000);

			// Process the received data
			pp = pp_start(dg->payload, dg->length);
			while(pp.ptr < pp.len) {
				uint32_t x = pp_u32(&pp);
				printf("%d,", x);
			}
			printf("\n");

			// and ask for more
			readState->pos += dg->length;

			if (readState->pos >= readState->total) {
				dbg("Transfer is complete.");
				// transfer complete

				// make sure the peer has freed the buffer
				// (may be waiting for us if we wanted to re-read something)

				retry_TO(100, sbmp_bulk_abort(ep, dg->session));
				goto cleanup;
			} else {
				// read next part
				retry_TO(100, sbmp_bulk_request(ep, readState->pos, CHUNK_LEN, dg->session));
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
	free(readState);
	// remove the listener
	sbmp_ep_remove_listener(ep, dg->session);
	stopReadoutTmeoTimer();
	acquire_pending = false;

	// In case the error was in SBMP (bad state)
}


static bool FLASH_FN meas_request_data(uint16_t count)
{
	bool suc = false;

	info("Requesting data capture - %d samples.", count);

	if (acquire_pending) {
		error("Acquire request already in progress.");
		return false;
	}

	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		error("Hsk status not SUCCESS, can't request data.");
		return false;
	}

	acquire_pending = true;

	// start the abort timer - timeout
	setReadoutTmeoTimer(6000);

	// start a message
	uint16_t sesn = 0;
	retry_TO(100, sbmp_ep_start_message(dlnk_ep, DG_REQUEST_CAPTURE, sizeof(uint16_t), &sesn));
	if (!suc) goto fail;

	// register the session listener
	suc = sbmp_ep_add_listener(dlnk_ep, sesn, request_data_sesn_listener, NULL);
	if (!suc) {
		// discard the unfinished outgoing packet
		sbmp_frm_reset_tx(&dlnk_ep->frm);
		goto fail;
	}

	acquire_session = sesn;

	// request N values
	sbmp_ep_send_u16(dlnk_ep, count);

	dbg("Request sent, session nr %d", sesn);

	return true;

fail:
	stopReadoutTmeoTimer();
	acquire_pending = false;
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
