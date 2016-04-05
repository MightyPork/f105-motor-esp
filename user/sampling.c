#include <esp8266.h>
#include <sbmp.h>

#include "datalink.h"
#include "sampling.h"
#include "serial.h"

// The buffer is big enough for 256 data bytes - 4*64

// chunk size for bulk transfer (must be multiple of 4 - length of uint32 / float)
// NOTE: If too large, strange errors can occur (problem with the underlying UART FIFO at high speed)
// the FIFO has 128 bytes, and should accomodate ideally the whole frame.
#define CHUNK_LEN 100


// Only one readout can happen at a time.

static struct {
	bool waiting_for_measure; /*!< Still waiting for first data packet */
	bool pending;      /*!< Flag that data is currently being read */
	uint16_t sesn;     /*!< SBMP session of the readout sequence */
	bool chunk_ready;  /*!< Chunk was received and is ready for reading */
	uint8_t received_chunk[CHUNK_LEN];  /*!< Copy of the latest received chunk of data */
	uint16_t received_chunk_size;       /*!< Size of the chunk in latest_chunk_copy */

	uint32_t est_sampling_time; /*!< Estimated time in millis before data is captured and readout starts */

	uint32_t pos;
	uint32_t total;

	MEAS_FORMAT format; /*!< Requested data format */

	// --- data stats ---
	MeasStats stats;
} rd;


bool FLASH_FN meas_is_closed(void)
{
	return !rd.pending;
}


uint32_t FLASH_FN meas_estimate_duration(uint32_t count, uint32_t freq)
{
	return (uint32_t)((count*1000.0f) / freq) + SAMP_RD_TMEO;
}

/** Wait for one chunk, with possible retries */
bool FLASH_FN meas_wait_for_chunk(void)
{
	for (int retry_count = 0; retry_count < SAMP_RD_RETRY_COUNT; retry_count++) {
		uint32_t timeout = (rd.waiting_for_measure ? rd.est_sampling_time: SAMP_RD_TMEO);
//		dbg("Chunk read total timeout = %d ms", timeout);

		for (uint32_t i = 0; i < timeout*100; i++) {
			uart_poll(); // can stop measure & start first chunk, if rx offer

			// check for closed connection - aborted by peer?
			if (meas_is_closed()) {
				error("Session closed by peer, readout failed.");
				return false; // assume already cleaned up
			}

			if (meas_chunk_ready()) {
				// yay!!
				return true;
			}

			os_delay_us(10);
			system_soft_wdt_feed(); // Feed the dog, or it'll bite.
		}

		// Data still not Rx
		if (rd.waiting_for_measure) {

			// only one try in this case
			error("Sampling aborted due to timeout (no data offered)");
			sbmp_bulk_abort(dlnk_ep, rd.sesn); // send abort msg
			meas_close(); // close
			return false;

		} else {
			warn("Data chunk not rx in time.");
			dbg("Requesting again (try %d of %d).", retry_count+1, SAMP_RD_RETRY_COUNT);

			sbmp_bulk_request(dlnk_ep, rd.pos, CHUNK_LEN, rd.sesn);
		}
	}

	error("Retry count exhausted!");
	sbmp_bulk_abort(dlnk_ep, rd.sesn);
	meas_close();
	return false;
}

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
	if (!rd.pending) return; // ignore this call

	sbmp_ep_remove_listener(dlnk_ep, rd.sesn);
	rd.pending = false;

	info("Transfer closed.");
}

MeasStats FLASH_FN *meas_get_stats(void)
{
	return &rd.stats;
}

static void FLASH_FN request_data_sesn_listener(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	(void)obj;

//	dbg("Received msg in session %d, dg type %d", dg->session, dg->type);

	PayloadParser pp;
	switch (dg->type) {
		case DG_BULK_OFFER:// Data ready notification
			// data is ready to be read
			pp = pp_start(dg->payload, dg->length);

			rd.pos = 0;
			rd.total = pp_u32(&pp);
			rd.waiting_for_measure = false; // "pending" flag remains set

			// --- here start the user data (common) ---
			rd.stats.count = pp_u32(&pp);
			rd.stats.freq = pp_float(&pp);
			rd.stats.min = pp_float(&pp);
			rd.stats.max = pp_float(&pp);
			rd.stats.rms = pp_float(&pp);
			// --- user data end ---

			if (rd.format == FFT) {
				// TODO read extra FFT stats ??
			}

			info("Offered %d bytes of data, starting readout.", rd.total);

			// request first chunk
			sbmp_bulk_request(ep, rd.pos, CHUNK_LEN, dg->session);
			break;

		case DG_BULK_DATA: // data received
			// Process the received data
			memcpy(rd.received_chunk, dg->payload, dg->length);
			rd.chunk_ready = true;
			rd.received_chunk_size = dg->length;

			// move the pointer for next request
			rd.pos += dg->length;

			// --- Now we wait for the CGI func to retrieve the chunk and send it to the browser. ---

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


bool FLASH_FN meas_request_data(MEAS_FORMAT format, uint16_t count, uint32_t freq)
{
	bool suc = false;

	info("Requesting data capture - %d samples @ %d Hz, fmt %d.", count, freq, format);

	if (rd.pending) {
		warn("Acquire request already in progress; shouldn't happen, IGNORING");
	}

	if (sbmp_ep_handshake_status(dlnk_ep) != SBMP_HSK_SUCCESS) {
		error("Hsk status not SUCCESS, can't request data.");
		return false;
	}

	// clean up
	rd.chunk_ready = false;
	rd.pos = 0;
	rd.total = 0;
	rd.pending = true;
	rd.format = format;
	memset(&rd.stats, 0, sizeof(MeasStats)); // clear the stats obj

	rd.est_sampling_time = meas_estimate_duration(count, freq);

	// start a message
	uint16_t sesn = 0;
	suc = sbmp_ep_start_message(dlnk_ep, format, sizeof(uint16_t)+sizeof(uint32_t), &sesn); // format enum matches the message types
	if (!suc) goto fail;

	// register the session listener
	suc = sbmp_ep_add_listener(dlnk_ep, sesn, request_data_sesn_listener, NULL);
	if (!suc) {
		// discard the unfinished outgoing packet
		sbmp_frm_reset_tx(&dlnk_ep->frm);
		goto fail;
	}

	rd.sesn = sesn;
	rd.waiting_for_measure = true; // waiting for acquisition module to start sending data

	// request N values
	sbmp_ep_send_u16(dlnk_ep, count);

	// at freq F
	sbmp_ep_send_u32(dlnk_ep, freq);

	dbg("Request sent, session nr %d", sesn);

	return true;

fail:
	rd.waiting_for_measure = false;
	rd.pending = false;
	return false;
}
