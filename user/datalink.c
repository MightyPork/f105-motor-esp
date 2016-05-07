#include "uart_driver.h"
#include "datalink.h"
#include "wificontrol.h"

// payload rx buffer
#define PAYLOAD_BUFFER_LEN (256+3)


SBMP_Endpoint *dlnk_ep;


/** func used for sending bytes by SBMP */
static void FLASH_FN u0_putc(uint8_t c)
{
	UART_WriteChar(UART0, c, 0);
}


/** Handle incoming datagram (new session) */
static void FLASH_FN dg_handler(SBMP_Datagram *dg)
{
	switch (dg->type) {
		case DG_SETMODE_AP:
			wificontrol_setmode_ap();
			break;

		case DG_WPS_START:
			wificontrol_start_wps();
			break;
	}
}

/** This is called by the UART rx handler */
void FLASH_FN datalink_receive(uint8_t byte)
{
	sbmp_ep_receive(dlnk_ep, byte);
}

/** Datalink */
void FLASH_FN datalinkInit(void)
{
	dlnk_ep = sbmp_ep_init(NULL, NULL, PAYLOAD_BUFFER_LEN, dg_handler, u0_putc);
	sbmp_ep_init_listeners(dlnk_ep, NULL, 4);

	sbmp_ep_enable(dlnk_ep, true);

	info("SBMP started on UART0");
}
