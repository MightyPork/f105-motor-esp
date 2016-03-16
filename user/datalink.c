#include "esp8266.h"
#include "uart_driver.h"
#include "sbmp.h"

#include "datalink.h"

/** func used for sending bytes by SBMP */
static void u0_putc(uint8_t c)
{
	UART_WriteCharCRLF(UART0, c, 0);
}


static void dg_handler(SBMP_Datagram *dg)
{
	sbmp_info("Datagram received.");
}



static SBMP_Endpoint *ep;

/** Datalink */
void datalinkInit(void)
{
	ep = sbmp_ep_init(NULL, NULL, 256, dg_handler, u0_putc);
}
