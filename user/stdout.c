//Stupid bit of code that does the bare minimum to make os_printf work.

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <esp8266.h>
#include "uart.h"


///** USART1 write routine */
//static void ICACHE_FLASH_ATTR stdoutUart1Txd(char c)
//{
//	//Wait until there is room in the FIFO
//	while (((READ_PERI_REG(UART_STATUS(0)) >> UART_TXFIFO_CNT_S) & UART_TXFIFO_CNT) >= 126) ;
//	//Send the character
//	WRITE_PERI_REG(UART_FIFO(0), c);
//}


///** Putchar to USART1 */
//static void ICACHE_FLASH_ATTR stdoutPutcharUSART1(char c)
//{
//	//convert \n -> \r\n
//	if (c == '\n') stdoutUart1Txd('\r');
//	stdoutUart1Txd(c);
//}


/**
 * @brief Configure UART 115200-8-N-1
 * @param uart_no
 */
void ICACHE_FLASH_ATTR
my_uart_init(UARTn uart_no)
{
	UART_SetParity(uart_no, PARITY_NONE);
	UART_SetStopBits(uart_no, ONE_STOP_BIT);
	UART_SetWordLength(uart_no, EIGHT_BITS);
	UART_SetBaudrate(uart_no, BIT_RATE_115200);
	UART_ResetFifo(uart_no);
}


void ICACHE_FLASH_ATTR
stdoutInit()
{
	// U0TXD
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);

	// U0RXD
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);

	// U1TXD (GPIO2)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);

	// Configure the UART peripherals
	my_uart_init(UART0); // main
	my_uart_init(UART1); // debug (output only)

	// Select debug port
	UART_SetPrintPort(UART1);
}
