#ifndef SAMPLING_H
#define SAMPLING_H

#include <esp8266.h>
#include <httpd.h>

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
bool meas_request_data(uint16_t count); // TODO specify what kind of data - currently direct samples.

/** request next chunk */
void meas_request_next_chunk(void);

/** Check if chunk ready to be read */
bool meas_chunk_ready(void);

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
