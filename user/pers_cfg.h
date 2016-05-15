#ifndef REPORTING_H
#define REPORTING_H

#include <esp8266.h>

typedef struct {

	// ...

	uint32_t magic;
} PersistentCfg;


/** Reporting config struct */
extern PersistentCfg pers_conf;

/** Save reporting config to flash */
void persistent_cfg_save(void);

/** Load the reporting config from flash */
void persistent_cfg_load(void);

#endif // REPORTING_H
