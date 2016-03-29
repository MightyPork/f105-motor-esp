#ifndef SAMPLING_H
#define SAMPLING_H

#include <esp8266.h>
#include <httpd.h>
#include "datalink.h"

#define SAMPLING_TMEO 6000
#define SAMP_READOUT_TMEO 100

typedef struct {
	uint32_t count;
	float freq; // actual frequency - not exact due to the prescaller limitations
	float min;
	float max;
	float rms;
} MeasStats;

typedef enum {
	RAW = DG_REQUEST_RAW, // same as the SBMP packet numbers used to request it
	FFT = DG_REQUEST_FFT
} MEAS_FORMAT;


/**
 * Reading procedure
 * -----------------
 *
 * 1. meas_request_data(count)
 * 2. wait for meas_chunk_ready() == true
 * 3. meas_get_chunk() to read the chunk last received
 * 4. if meas_is_last_chunk(), call meas_close() and DONE.
 * 5. meas_request_next_chunk() to get more data, goto 2
 *
 * Waiting should have a timeout, if data not received, return error status to client.
 */

/** Request data from the sampling module. Count - number of samples. */
bool meas_request_data(MEAS_FORMAT format, uint16_t count, uint32_t freq); // TODO specify what kind of data - currently direct samples.

/** request next chunk */
void meas_request_next_chunk(void);

/** Check if chunk ready to be read */
bool meas_chunk_ready(void);

/** Check if closed (if data was expected, this means the peer aborted the transaction) */
bool meas_is_closed(void);

/** Get the stats struct */
MeasStats *meas_get_stats(void);

/**
 * @brief Get received chunk. NULL if none.
 *
 * The array is valid until next chunk is requested.
 * Chunk length in bytes is stored in the argument.
 */
uint8_t *meas_get_chunk(uint16_t *chunk_len);

/** Check if this was the last chunk */
bool meas_is_last_chunk(void);

/** Terminate the readout. */
void meas_close(void);


#endif // SAMPLING_H
