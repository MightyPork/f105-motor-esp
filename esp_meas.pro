TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES = ESPFS_HEATSHRINK HTTPD_MAX_CONNECTIONS=4 __ets__

INCLUDEPATH = . \
	esp_iot_sdk_v1.5.2/include \
	include \
	user \
	libesphttpd/include \
	libesphttpd/espfs \
	libesphttpd/core \
	libesphttpd/lib/heatshrink \
	sbmp/library

SOURCES += \
	libesphttpd/core/auth.c \
	libesphttpd/core/base64.c \
	libesphttpd/core/httpd-freertos.c \
	libesphttpd/core/httpd-nonos.c \
	libesphttpd/core/httpd.c \
	libesphttpd/core/httpdespfs.c \
	libesphttpd/core/sha1.c \
	libesphttpd/espfs/espfstest/main.c \
	libesphttpd/espfs/mkespfsimage/heatshrink_encoder.c \
	libesphttpd/espfs/mkespfsimage/main.c \
	libesphttpd/espfs/espfs.c \
	libesphttpd/espfs/heatshrink_decoder.c \
	libesphttpd/lib/heatshrink/heatshrink.c \
	libesphttpd/lib/heatshrink/heatshrink_decoder.c \
	libesphttpd/lib/heatshrink/heatshrink_encoder.c \
	libesphttpd/lib/heatshrink/test_heatshrink_dynamic.c \
	libesphttpd/lib/heatshrink/test_heatshrink_dynamic_theft.c \
	libesphttpd/lib/heatshrink/test_heatshrink_static.c \
	libesphttpd/mkupgimg/mkupgimg.c \
	libesphttpd/util/captdns.c \
	libesphttpd/util/cgiflash.c \
	libesphttpd/util/cgiwebsocket.c \
	libesphttpd/util/cgiwifi.c \
	user/cgi-test.c \
	user/cgi.c \
	user/io.c \
	user/user_main.c \
	user/uart_driver.c \
	sbmp/library/crc32.c \
	sbmp/library/sbmp_checksum.c \
	sbmp/library/sbmp_datagram.c \
	sbmp/library/sbmp_frame.c \
	sbmp/library/sbmp_session.c \
	sbmp/library/sbmp_bulk.c \
	user/datalink.c \
	user/serial.c \
	user/uptime.c \
	sbmp/library/payload_parser.c \
	user/sampling.c \
	user/page_home.c

HEADERS += \
	include/uart_hw.h \
	include/user_config.h \
	libesphttpd/core/base64.h \
	libesphttpd/core/httpd-platform.h \
	libesphttpd/espfs/espfsformat.h \
	libesphttpd/espfs/heatshrink_config_custom.h \
	libesphttpd/include/auth.h \
	libesphttpd/include/captdns.h \
	libesphttpd/include/cgiflash.h \
	libesphttpd/include/cgiwebsocket.h \
	libesphttpd/include/cgiwifi.h \
	libesphttpd/include/esp8266.h \
	libesphttpd/include/espfs.h \
	libesphttpd/include/espmissingincludes.h \
	libesphttpd/include/httpd.h \
	libesphttpd/include/httpdespfs.h \
	libesphttpd/include/platform.h \
	libesphttpd/include/sha1.h \
	libesphttpd/include/user_config.h \
	libesphttpd/include/webpages-espfs.h \
	libesphttpd/lib/heatshrink/greatest.h \
	libesphttpd/lib/heatshrink/heatshrink_common.h \
	libesphttpd/lib/heatshrink/heatshrink_config.h \
	libesphttpd/lib/heatshrink/heatshrink_decoder.h \
	libesphttpd/lib/heatshrink/heatshrink_encoder.h \
	user/cgi-test.h \
	user/cgi.h \
	user/io.h \
	user/uart_register.h \
	esp_iot_sdk_v1.5.2/include/json/json.h \
	esp_iot_sdk_v1.5.2/include/json/jsonparse.h \
	esp_iot_sdk_v1.5.2/include/json/jsontree.h \
	esp_iot_sdk_v1.5.2/include/airkiss.h \
	esp_iot_sdk_v1.5.2/include/at_custom.h \
	esp_iot_sdk_v1.5.2/include/c_types.h \
	esp_iot_sdk_v1.5.2/include/eagle_soc.h \
	esp_iot_sdk_v1.5.2/include/esp_sdk_ver.h \
	esp_iot_sdk_v1.5.2/include/espconn.h \
	esp_iot_sdk_v1.5.2/include/espnow.h \
	esp_iot_sdk_v1.5.2/include/ets_sys.h \
	esp_iot_sdk_v1.5.2/include/gpio.h \
	esp_iot_sdk_v1.5.2/include/ip_addr.h \
	esp_iot_sdk_v1.5.2/include/mem.h \
	esp_iot_sdk_v1.5.2/include/mesh.h \
	esp_iot_sdk_v1.5.2/include/os_type.h \
	esp_iot_sdk_v1.5.2/include/osapi.h \
	esp_iot_sdk_v1.5.2/include/ping.h \
	esp_iot_sdk_v1.5.2/include/pwm.h \
	esp_iot_sdk_v1.5.2/include/queue.h \
	esp_iot_sdk_v1.5.2/include/slc_register.h \
	esp_iot_sdk_v1.5.2/include/smartconfig.h \
	esp_iot_sdk_v1.5.2/include/sntp.h \
	esp_iot_sdk_v1.5.2/include/spi_flash.h \
	esp_iot_sdk_v1.5.2/include/spi_register.h \
	esp_iot_sdk_v1.5.2/include/uart_register.h \
	esp_iot_sdk_v1.5.2/include/upgrade.h \
	esp_iot_sdk_v1.5.2/include/user_interface.h \
	user/uart_driver.h \
	sbmp/library/crc32.h \
	sbmp/library/sbmp.h \
	sbmp/library/sbmp_checksum.h \
	sbmp/library/sbmp_config.h \
	sbmp/library/sbmp_datagram.h \
	sbmp/library/sbmp_frame.h \
	sbmp/library/sbmp_logging.h \
	sbmp/library/sbmp_session.h \
	sbmp/library/sbmp_bulk.h \
	user/datalink.h \
	user/serial.h \
	libesphttpd/include/logging.h \
	user/uptime.h \
	sbmp/library/payload_parser.h \
	user/sampling.h \
	user/page_home.h \
	user/timeout.h \
	user/sbmp_config.h \
	sbmp/library/sbmp_config.example.h

DISTFILES += \
	style.astylerc \
	Makefile
