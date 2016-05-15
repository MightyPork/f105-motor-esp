#include "pers_cfg.h"
#include "datalink.h"
#include "serial.h"
#include "httpclient.h"
#include "ftoa.h"

#define RPT_CONF_MAGIC 0x24C595D5

PersistentCfg pers_conf;

/** Save reporting config to flash */
void FLASH_FN persistent_cfg_save(void)
{
	info("Saving persistent user config");

	system_param_save_with_protect(0x3D, &pers_conf, sizeof(PersistentCfg));

	info("Config saved.");
}

/** Load the reporting config from flash */
void FLASH_FN persistent_cfg_load(void)
{
	info("Loading persistent user config");

	system_param_load(0x3D, 0, &pers_conf, sizeof(PersistentCfg));

	if (pers_conf.magic != RPT_CONF_MAGIC) {
		warn("Config block corrupted, reset to defaults.");

		// invalid config, zero out
		memset(&pers_conf, 0, sizeof(PersistentCfg));
		pers_conf.magic = RPT_CONF_MAGIC;

		// save fixed
		persistent_cfg_save();
	}

	info("Config loaded.");
}
