
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


// Edited: do not fuck with GPIO2, it's used for debug usart output

// GPIO0 remains used as reset btn.



#include <esp8266.h>

//#define LEDGPIO 2
#define BTNGPIO 0

static ETSTimer resetBtntimer;

//void ICACHE_FLASH_ATTR ioLed(int ena) {
//	//gpio_output_set is overkill. ToDo: use better mactos
//	if (ena) {
//		gpio_output_set((1<<LEDGPIO), 0, (1<<LEDGPIO), 0);
//	} else {
//		gpio_output_set(0, (1<<LEDGPIO), (1<<LEDGPIO), 0);
//	}
//}

static void FLASH_FN resetBtnTimerCb(void *arg) {
	static int resetCnt=0;
	if (!GPIO_INPUT_GET(BTNGPIO)) {
		resetCnt++;
	} else {
		if (resetCnt>=6) { //3 sec pressed
			wifi_station_disconnect();
			wifi_set_opmode(STATIONAP_MODE); //reset to AP+STA mode
			os_printf("Reset to AP mode. Restarting system...\n");
			system_restart();
		}
		resetCnt=0;
	}
}

void FLASH_FN ioInit() {
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	gpio_output_set(0, 0, 0/*(1<<LEDGPIO)*/, (1<<BTNGPIO));

	if (GPIO_INPUT_GET(BTNGPIO) == 0) {
		// starting "in BOOT mode" - do not install the AP reset timer
		os_printf("GPIO0 stuck low - AP reset button disabled.\n");
	} else {
		os_timer_disarm(&resetBtntimer);
		os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
		os_timer_arm(&resetBtntimer, 500, 1);

		os_printf("Note: Hold GPIO0 low for reset to AP mode.\n");
	}
}

