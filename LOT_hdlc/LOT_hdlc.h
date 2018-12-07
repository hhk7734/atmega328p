/**
 * @file LOT_hdlc.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief High level Data Link Control
 */

#ifndef _LOT_HDLC_H_
#define _LOT_HDLC_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
#include "LOT_uart0.h"
#define LOT_hdlc_write( x ) LOT_uart0_write( x )
#define LOT_hdlc_internal_available() LOT_uart0_available()
#define LOT_hdlc_read() LOT_uart0_read()
/// 저장 가능한 프레임 수
#define LOT_HDLC_BUF_SIZE 4
/// 프레임 내의 가변 데이터 최대 개수
#define LOT_HDLC_DATA_SIZE 20
/**
 * @}
 */

#define LOT_HDLC_START_END_FLAG 0x7E
#define LOT_HDLC_ESCAPE_FLAG 0x7D
#define LOT_HDLC_ESCAPE_XOR_VALUE 0x20

void    LOT_hdlc_transmit( const uint8_t *data, uint8_t count );
void    LOT_hdlc_recieve( void );
uint8_t LOT_hdlc_available( void );
void    LOT_hdlc_get_buf( uint8_t **data, uint8_t *count );

#ifdef __cplusplus
}
#endif
#endif // _LOT_HDLC_H_
