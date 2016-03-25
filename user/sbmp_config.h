#ifndef SBMP_CONFIG_H
#define SBMP_CONFIG_H

#include <esp8266.h>


/* ---------- CRC32 ---------------- */

/**
 * @brief Add support for CRC32
 *
 * Disabling CRC32 will reduce program size (for small micros).
 * If CRC32 is disabled, XOR will be used as the preferred checksum
 * method.
 *
 * Received CRC32'd messages will be accepted without checking.
 *
 * If handshake is used, the peer will detect that CRC32 is not
 * supported here, and should start using XOR.
 */
#define SBMP_HAS_CRC32 1


/* ---------- MALLOC --------------- */

/**
 * @brief Enable malloc if NULL is passed.
 *
 * This lets you malloc() the struct / buffer if you pass NULL
 * to the init functions.
 *
 * Disable malloc to free up memory taken by the malloc routine.
 * If disabled, init funcs will return NULL if NULL is passed
 * as argument.
 */
#define SBMP_USE_MALLOC 1

// those will be used if malloc is enabled
#define sbmp_malloc(n)   os_malloc(n)
#define sbmp_free(x)     os_free(x)
#define sbmp_calloc(n,l) os_zalloc((n)*(l))


/* ---------- LOGGING -------------- */

/**
 * @brief Enable logging.
 *
 * Logging functions are WEAK stubs in sbmp_logging.
 *
 * Disable logging to free up memory taken by the messages.
 */
#define SBMP_LOGGING 1

/**
 * @brief Enable detailed logging (only for debugging, disable for better performance).
 */
#define SBMP_DEBUG 0

// here are the actual logging functions

#if SBMP_DEBUG||SBMP_LOGGING
# define sbmp_error(fmt, ...) error("[SBMP] "fmt, ##__VA_ARGS__)
# define sbmp_warn(fmt, ...)  warn("[SBMP] "fmt, ##__VA_ARGS__)
# define sbmp_info(fmt, ...)  info("[SBMP] "fmt, ##__VA_ARGS__)
#else
# define sbmp_error(fmt, ...)
# define sbmp_warn(fmt, ...)
# define sbmp_info(fmt, ...)
#endif

#if SBMP_DEBUG
# define sbmp_dbg(fmt, ...)   dbg("[SBMP] "fmt, ##__VA_ARGS__)
#else
# define sbmp_dbg(fmt, ...)
#endif


/* ---------- Fix formatting -------------- */

#define PRIu16 "u"
#define PRIu32 "u"
#define PRIu8  "u"
#define PRIi16 "d"
#define PRIi32 "d"
#define PRIi8  "d"

#endif // SBMP_CONFIG_H
