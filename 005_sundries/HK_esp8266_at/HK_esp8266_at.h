/**
 * filename : LOT_esp8266_at.h
 *
 * created  : 2018/10/02
 *
 * Hyeon-ki, Hong
 * hhk7734@gmail.com
 *
 * purpose : ESP8266 AT commands
 */

/**
 * test
 * AT version:1.2.0.0(Jul  1 2016 20:04:45)
 * SDK version:1.5.4.1(39cb9a32)
 */

#ifndef _LOT_ESP8266_AT_H_
#define _LOT_ESP8266_AT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#ifndef _LOT_STATUS_TYPEDEF_
#define _LOT_STATUS_TYPEDEF_
typedef enum
{
    LOT_OK = 0x00,
    LOT_ERROR,
    LOT_BUSY,
    LOT_TIMEOUT
} LOT_status_typedef;
#endif

#define LOT_ESP8266_AT_MAX_TRY 5
#define LOT_DEBUG_SERIAL_WRITE( _char )

#define LOT_esp8266_at_begin( _baudrate )
#define LOT_esp8266_at_read()
#define LOT_esp8266_at_write( _char )
#define LOT_esp8266_at_print_str( _str )
#define LOT_esp8266_at_available()
#define LOT_esp8266_at_flush()

LOT_status_typedef LOT_esp8266_at_find( const char *_str, uint32_t timeout_ms );
LOT_status_typedef LOT_esp8266_at_test( void );

#ifdef __cplusplus
}
#endif
#endif // _LOT_ESP8266_AT_H_