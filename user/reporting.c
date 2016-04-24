#include "reporting.h"
#include "datalink.h"
#include "serial.h"

#define RPT_CONF_MAGIC 0x24C595D5

ReportingResult rpt_result;
ReportingCfg rpt_conf;

static os_timer_t rpt_tim;

/** Timer cb */
static void rpt_tim_cb(void *arg)
{
	(void)arg;
	reporting_send_now();
}

/** Stop / start timer & set interval based on rpt conf */
static void set_timer(void)
{
	os_timer_disarm(&rpt_tim);

	if (rpt_conf.enabled) {
		os_timer_setfn(&rpt_tim, rpt_tim_cb, NULL);
		os_timer_arm(&rpt_tim, (int)(rpt_conf.interval*1000), 1);
	}
}

/** Fix unterminated strings, add magic, etc.. */
static void normalize_rpt_conf(void)
{
	// terminate strings
	rpt_conf.feed[sizeof(rpt_conf.feed)-1] = 0;
	rpt_conf.key[sizeof(rpt_conf.key)-1] = 0;
	// set magic
	rpt_conf.magic = RPT_CONF_MAGIC;
}

/** Save reporting config to flash */
void reporting_save(void)
{
	normalize_rpt_conf(); // fix weirdness
	system_param_save_with_protect(0x3D, &rpt_conf, sizeof(ReportingCfg));

	// start timer for the new interval time
	set_timer();
}

/** Load the reporting config from flash */
void reporting_load(void)
{
	system_param_load(0x3D, 0, &rpt_conf, sizeof(ReportingCfg));

	if (rpt_conf.magic != RPT_CONF_MAGIC) {
		// invalid config, zero out
		memset(&rpt_conf, 0, sizeof(ReportingCfg));
		rpt_conf.magic = RPT_CONF_MAGIC;

		// save fixed
		reporting_save();
	}

	set_timer();
}

void compare_ref_cb(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	(void)obj;
	sbmp_ep_remove_listener(ep, dg->session);

	PayloadParser pp = pp_start(dg->payload, dg->length);
	rpt_result.deviation = pp_float(&pp);
	rpt_result.rms = pp_float(&pp);
}

/** Immediately send report to xively / thingspeak */
void reporting_send_now(void)
{
	uint16_t sesn;
	sbmp_ep_send_message(dlnk_ep, DG_REQUEST_COMPARE_REF, NULL, 0, &sesn, NULL);
	sbmp_ep_add_listener(dlnk_ep, sesn, compare_ref_cb, NULL);

	// poll & wait for response
	const int timeout = 500;
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
}
