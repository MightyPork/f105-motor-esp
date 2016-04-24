#ifndef REPORTING_H
#define REPORTING_H

#include <esp8266.h>

typedef struct {
	// 0
	bool enabled : 4;
	// 4
	uint32_t interval;
	// 8
	enum {
		RPT_XIVELY,
		RPT_THINGSPEAK
	} service : 4;
	// 12
	char feed[64];
	// 76
	char key[64];
	// 80
	uint32_t magic;
} ReportingCfg;

/** Comapre result is stored here */
typedef struct {
	bool ready : 4;
	float deviation;
	float rms;
} ReportingResult;

extern ReportingResult rpt_result;

/** Reporting config struct */
extern ReportingCfg rpt_conf;

/** Save reporting config to flash */
void reporting_save(void);

/** Load the reporting config from flash */
void reporting_load(void);

/** Immediately send report to xively / thingspeak */
void reporting_send_now(void);

#endif // REPORTING_H
