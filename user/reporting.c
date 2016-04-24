#include "reporting.h"
#include "datalink.h"
#include "serial.h"
#include "httpclient.h"

#define RPT_CONF_MAGIC 0x24C595D5

ReportingResult rpt_result;
ReportingCfg rpt_conf;

static os_timer_t rpt_tim;

/** Timer cb */
static void FLASH_FN rpt_tim_cb(void *arg)
{
	(void)arg;
	// send report now...
	if (rpt_conf.enabled) {
		capture_and_report();
	}
}

/** Stop / start timer & set interval based on rpt conf */
static FLASH_FN void set_timer(void)
{
	os_timer_disarm(&rpt_tim);

	if (rpt_conf.enabled) {
		os_timer_setfn(&rpt_tim, rpt_tim_cb, NULL);
		os_timer_arm(&rpt_tim, (int)(rpt_conf.interval * 1000), 1);
	}
}

/** Fix unterminated strings, add magic, etc.. */
static FLASH_FN void normalize_rpt_conf(void)
{
	// terminate strings
	rpt_conf.feed[sizeof(rpt_conf.feed) - 1] = 0;
	rpt_conf.key[sizeof(rpt_conf.key) - 1] = 0;
	// set magic
	rpt_conf.magic = RPT_CONF_MAGIC;
}

static FLASH_FN void dump_rpt_conf(void)
{
	dbg("Enabled: %d | Interval: %d | Service: %s", rpt_conf.enabled, rpt_conf.interval, (rpt_conf.service == RPT_XIVELY ? "Xively" : "ThingSpeak"));
	dbg("Key: %s | Feed: %s", rpt_conf.key, rpt_conf.feed);
}

/** Save reporting config to flash */
void FLASH_FN reporting_cfg_save(void)
{
	normalize_rpt_conf(); // fix weirdness

	info("Saving monitoring config");
	dump_rpt_conf();

	system_param_save_with_protect(0x3D, &rpt_conf, sizeof(ReportingCfg));

	// start timer for the new interval time
	set_timer();

	info("Config saved.");
}

/** Load the reporting config from flash */
void FLASH_FN reporting_cfg_load(void)
{
	info("Loading monitoring config");

	system_param_load(0x3D, 0, &rpt_conf, sizeof(ReportingCfg));

	if (rpt_conf.magic != RPT_CONF_MAGIC) {
		warn("Config block corrupted, reset to defaults.");

		// invalid config, zero out
		memset(&rpt_conf, 0, sizeof(ReportingCfg));
		rpt_conf.magic = RPT_CONF_MAGIC;

		// save fixed
		reporting_cfg_save();
	} else {
		dump_rpt_conf();
	}

	set_timer();

	info("Config loaded.");
}

/** Called when response to Compare Ref is received */
static void FLASH_FN compare_ref_cb(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	(void)obj;
	sbmp_ep_remove_listener(ep, dg->session);

	info("Measurement complete.");

	rpt_result.success = (dg->type == DG_SUCCESS);

	if (dg->type == DG_SUCCESS) {
		PayloadParser pp = pp_start(dg->payload, dg->length);
		rpt_result.deviation = pp_float(&pp);
		rpt_result.i_rms = pp_float(&pp);
	} else {
		error("FAIL resp from sbmp.");
	}

	rpt_result.ready = true; // signal to waiting loop
}


/** Send report from rpt_result */
static void FLASH_FN do_send_report(void)
{
	info("Sending report...");
	switch (rpt_conf.service) {
		case RPT_XIVELY:
			warn("------- TODO: REPORT TO XIVELY -------");
			break;

		case RPT_THINGSPEAK:
			warn("------- TODO: REPORT TO THINGSPEAK -------");
			break;
	}
}


/** Immediately send report to xively / thingspeak */
bool FLASH_FN capture_and_report(void)
{
	info("Starting reporting measurmenet...");
	rpt_result.ready = false;

	uint16_t sesn;
	sbmp_ep_send_message(dlnk_ep, DG_REQUEST_COMPARE_REF, NULL, 0, &sesn, NULL);
	sbmp_ep_add_listener(dlnk_ep, sesn, compare_ref_cb, NULL);

	// poll & wait for response
	const int timeout = 500;
	for (uint32_t i = 0; i < timeout * 100; i++) {
		uart_poll();

		if (rpt_result.ready) {
			if (rpt_result.success) {
				do_send_report();
			}
			return true; // done
		}

		os_delay_us(10);
		system_soft_wdt_feed(); // Feed the dog, or it'll bite.
	}

	// timeout - remove listener
	error("Measure timeout - no resp received.");
	sbmp_ep_remove_listener(dlnk_ep, sesn);
	return false;
}


static bool capt_ref_done;
static bool capt_ref_success;

/** Callback for "store ref" */
static void FLASH_FN store_ref_cb(SBMP_Endpoint *ep, SBMP_Datagram *dg, void **obj)
{
	(void)obj;
	sbmp_ep_remove_listener(ep, dg->session);

	capt_ref_done = true;
	capt_ref_success = (dg->type == DG_SUCCESS);
}

/** Capture reference vector for monitoring */
bool FLASH_FN capture_reporting_reference(void)
{
	info("Capturing reference...");

	uint16_t sesn;
	sbmp_ep_send_message(dlnk_ep, DG_REQUEST_STORE_REF, NULL, 0, &sesn, NULL);
	sbmp_ep_add_listener(dlnk_ep, sesn, store_ref_cb, NULL);

	capt_ref_done = false;
	capt_ref_success = false;

	const int timeout = 500;
	for (uint32_t i = 0; i < timeout * 100; i++) {
		uart_poll();

		if (capt_ref_done) {
			return capt_ref_success; // done
		}

		os_delay_us(10);
		system_soft_wdt_feed(); // Feed the dog, or it'll bite.
	}

	// timeout - remove listener
	error("Ref capture timeout - no resp received.");
	sbmp_ep_remove_listener(dlnk_ep, sesn);
	return false;
}
