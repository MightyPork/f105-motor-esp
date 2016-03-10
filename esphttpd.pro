TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES =

INCLUDEPATH = . \
	esp_iot_sdk_v1.5.2/include \
	include \
	libesphttpd/include \
	libesphttpd/espfs

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
	user/stdout.c \
	user/user_main.c \
	user/uart.c

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
	user/stdout.h \
	user/uart.h \
	user/uart_register.h

DISTFILES += \
	style.astylerc
