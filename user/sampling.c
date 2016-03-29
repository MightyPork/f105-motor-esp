#include <esp8266.h>
#include <sbmp.h>

#include "datalink.h"
#include "sampling.h"

// The buffer is big enough for 256 data bytes - 4*64

// chunk size for bulk transfer (must be multiple of 4 - length of uint32 / float)
// NOTE: If too large, strange errors can occur (problem with the underlying UART FIFO at high speed)
// the FIFO has 128 bytes, and should accomodate ideally the whole frame.
#define CHUNK_LEN 100

#define SAMPLING_TMEO 10000
#define READOUT_TMEO 100

// Only one readout can happen at a time.

static struct {
	bool pending;                       /*!< Flag that data is currently being read */
	uint16_t sesn;                      /*!< SBMP session of the readout sequence */
	bool chunk_ready;                   /*!< Chunk was received and is ready for reading */
	uint8_t received_chunk[CHUNK_LEN];  /*!< Copy of the latest received chunk of data */
	uint16_t received_chunk_size;            /*!< Size of the chunk in latest_chunk_copy */

	// the readout state
	uint32_t pos;
	uint32_t total;
	ETSTimer abortTimer;
} rd;


// --- timeout ---

static void FLASH_FN abortTimerCb(void *arg)
{
	(void)arg;
	warn("Sampling aborted due to timeout.");

	// try to abort the readout
	sbmp_bulk_abort(dlnk_ep, rd.sesn);

	// free the data obj if not NULL
	sbmp_ep_free_listener_obj(dlnk_ep, rd.sesn);
	// release the slot
	sbmp_ep_remove_listener(dlnk_ep, rd.sesn);

	// invalidate the chunk buffer and indicate that a new readout can start
	rd.pending = false;
}

static void FLASH_FN setReadoutTmeoTimer(int ms)
{
//	dbg("Set read timeout %d", ms);
	os_timer_disarm(&rd.abortTimer);
	os_timer_setfn(&rd.abortTimer, abortTimerCb, NULL);
	os_timer_arm(&rd.abortTimer, ms, 0);
}

static void FLASH_FN stopReadoutTmeoTimer(void)
{
//	dbg("Stop read timeout");
	os_timer_disarm(&rd.abortTimer);
}

// -------------



/** request next chunk */
void FLASH_FN meas_request_next_chunk(void)
{
	if (!rd.pending) return;
	rd.chunk_ready = false; // invalidate the current chunk, so waiting for chunk is possible.
	sbmp_bulk_request(dlnk_ep, rd.pos, CHUNK_LEN, rd.sesn);
}

/** Check if chunk ready to be read */
bool FLASH_FN meas_chunk_ready(void)
{
	return rd.pending && rd.chunk_ready;
}

/**
 * @brief Get received chunk. NULL if none.
 *
 * The array is valid until next chunk is requested.
 * Chunk length in bytes is stored in the argument.
 */
uint8_t FLASH_FN *meas_get_chunk(uint16_t *chunk_len)
{
	if (!rd.pending) {
		warn("Meas not pending, cant get chunk");
		return NULL;
	}
	if (!rd.chunk_ready) {
		warn("Rx chunk not ready");
		return NULL;
	}

	*chunk_len = rd.received_chunk_size;
	return rd.received_chunk;
}

/** Check if this was the last chunk */
bool FLASH_FN meas_is_last_chunk(void)
{
	return rd.pos >= rd.total;
}

/** Terminate the readout. */
void FLASH_FN meas_close(void)
{
	sbmp_ep_remove_listener(dlnk_ep, rd.sesn);
	stopReadoutTmeoTimer();
	rd.pending = false;

	info("Transfer closed.");
}


static void FLASH_FN request_data_sesn_listener(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	(void)obj;

//	dbg("Received msg in session %d, dg type %d", dg->session, dg->type);

	PayloadParser pp;
	switch (dg->type) {
		case DG_BULK_OFFER:// Data ready notification
			stopReadoutTmeoTimer();
//			info("--- Peer offers data for bulk transfer ---");

			// data is ready to be read
			pp = pp_start(dg->payload, dg->length);

			rd.pos = 0;
			rd.total = pp_u32(&pp);

//			dbg("Total bytes avail: %d", rd.total);

			// renew the timeout
			setReadoutTmeoTimer(READOUT_TMEO);

			// request first chunk
			sbmp_bulk_request(ep, rd.pos, CHUNK_LEN, dg->session);
			break;

		case DG_BULK_DATA: // data received
			stopReadoutTmeoTimer();
//			info("--- Received a chunk, length %d ---", dg->length);

			// Process the received data
			memcpy(rd.received_chunk, dg->payload, dg->length);
			rd.chunk_ready = true;
			rd.received_chunk_size = dg->length;

			// move the pointer for next request
			rd.pos += dg->length;

			setReadoutTmeoTimer(READOUT_TMEO); // timeout to retrieve the data & ask for more

			if (rd.pos >= rd.total) {
				info("Transfer is complete.");
				// transfer complete

				// ask peer to release the buffer & go idle
				sbmp_bulk_abort(ep, dg->session);
			}
			break;

		case DG_BULK_ABORT: // Peer resets the readout
			// this is unlikely
			warn("Bulk transfer aborted by peer.");
			goto cleanup;
	}

	return;

cleanup:
	// remove the listener
	meas_close();
}


bool FLASH_FN meas_request_data(uint16_t count, uint32_t freq)
{
	bool suc = false;

	info("Requesting data capture - %d samples @ %d Hz.", count, freq);

	if (rd.pending) {
		error("Acquire request already in progress.");
		return false;
	}

	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		error("Hsk status not SUCCESS, can't request data.");
		return false;
	}

	rd.chunk_ready = false;
	rd.pos = 0;
	rd.total = 0;
	rd.pending = true;

	// start the abort timer - timeout
	setReadoutTmeoTimer(SAMPLING_TMEO);

	// start a message
	uint16_t sesn = 0;
	suc = sbmp_ep_start_message(dlnk_ep, DG_REQUEST_CAPTURE, sizeof(uint16_t)+sizeof(uint32_t), &sesn);
	if (!suc) goto fail;

	// register the session listener
	suc = sbmp_ep_add_listener(dlnk_ep, sesn, request_data_sesn_listener, NULL);
	if (!suc) {
		// discard the unfinished outgoing packet
		sbmp_frm_reset_tx(&dlnk_ep->frm);
		goto fail;
	}

	rd.sesn = sesn;

	// request N values
	sbmp_ep_send_u16(dlnk_ep, count);

	// at freq F
	sbmp_ep_send_u32(dlnk_ep, freq);

	dbg("Request sent, session nr %d", sesn);

	return true;

fail:
	stopReadoutTmeoTimer();
	rd.pending = false;
	return false;
}


// ------ C G I ---------


/*
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
*/
