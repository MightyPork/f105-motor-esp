#include "uart_driver.h"
#include "datalink.h"

// payload rx buffer
#define PAYLOAD_BUFFER_LEN (256+3)


SBMP_Endpoint *dlnk_ep;


/** func used for sending bytes by SBMP */
static void FLASH_FN u0_putc(uint8_t c)
{
	UART_WriteChar(UART0, c, 0);
}


static void FLASH_FN dg_handler(SBMP_Datagram *dg)
{
	dbg("[SBMP] Datagram received, type %d, session %d", dg->type, dg->session);
}

/** This is called by the UART rx handler */
void datalink_receive(uint8_t byte)
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
