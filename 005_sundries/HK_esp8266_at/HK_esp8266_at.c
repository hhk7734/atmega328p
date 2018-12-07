/**
 * filename : LOT_esp8266_at.c
 *
 * created  : 2018/10/02
 *
 * Hyeon-ki, Hong
 * hhk7734@gmail.com
 *
 * purpose : ESP8266 AT commands
 */

#include "LOT_esp8266_at.h"

#include <avr/pgmspace.h>

LOT_status_typedef LOT_esp8266_at_find( const char *_str, uint32_t timeout_ms )
{
    uint8_t index      = 0;
    uint8_t str_length = 0;
    while ( *( _str + str_length ) != '\0' )
    {
        ++str_length;
    }

    uint8_t *buff = ( uint8_t * )malloc( str_length );
    for ( uint8_t i = 0; i < str_length; ++i )
    {
        buff[i] = 0;
    }

    // TODO timeout
    while ( timeout_ms-- )
    {
        if ( LOT_esp8266_at_available() )
        {
            uint8_t temp = LOT_esp8266_at_read();
            LOT_DEBUG_SERIAL_WRITE( temp );
            buff[index] = temp;
            if ( temp == _str[str_length - 1] )
            {
                uint8_t check = 0;
                for ( uint8_t i = 0; i < str_length; ++i )
                {
                    if ( buff[( index + 1 + i ) % str_length] == _str[i] )
                    {
                        ++check;
                    }
                }
                if ( check == str_length )
                {
                    free( buff );
                    return LOT_OK;
                }
            }
            index = ( index + 1 ) % str_length;
        }
    }
    free( buff );
    return LOT_ERROR;
}

LOT_status_typedef LOT_esp8266_at_test( void )
{
    uint8_t _try = 0;
    do
    {
        if ( _try > LOT_ESP8266_AT_MAX_TRY )
        {
            return LOT_ERROR;
        }
        LOT_esp8266_at_print_str( PSTR( "AT\n\r" ) );
        ++_try;
    } while ( LOT_esp8266_at_find( "OK\r\n", 10000 ) != LOT_OK );
    return LOT_OK;
}
