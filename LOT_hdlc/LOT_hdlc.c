/**
 * @file LOT_hdlc.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief High level Data Link Control
 */

#include "LOT_hdlc.h"
#include "LOT_crc_8_ccitt.h"

typedef enum
{
    LOT_HDLC_START = 0x00,
    LOT_HDLC_STOP,
    LOT_HDLC_ESCAPE,
} LOT_hdlc_status_typedef;

static LOT_hdlc_status_typedef LOT_hdlc_status                        = LOT_HDLC_STOP;
static uint8_t                 LOT_hdlc_old_data                      = LOT_HDLC_START_END_FLAG;
static uint8_t                 LOT_hdlc_buf_tail                      = 0;
static uint8_t                 LOT_hdlc_buf_head                      = 0;
static uint8_t                 LOT_hdlc_data_count[LOT_HDLC_BUF_SIZE] = { 0 };
/// FCS를 위해 1 byte 추가
static uint8_t LOT_hdlc_buf[LOT_HDLC_BUF_SIZE][LOT_HDLC_DATA_SIZE + 1] = { { 0 } };

/**
 * @brief HDLC 프레임 전송, 0x7E - 가변 데이터 - CRC-8 - 0x7E
 * @param uint8_t *data 전송할 데이터 배열
 * @param uint8_t count 전송할 데이터 수
 */
void LOT_hdlc_transmit( const uint8_t *data, uint8_t count )
{
    const uint8_t *data_ptr = data;
    uint8_t        _count   = count;
    LOT_hdlc_write( LOT_HDLC_START_END_FLAG );
    while ( count-- )
    {
        if ( ( *data_ptr == LOT_HDLC_START_END_FLAG ) || ( *data_ptr == LOT_HDLC_ESCAPE_FLAG ) )
        {
            LOT_hdlc_write( LOT_HDLC_ESCAPE_FLAG );
            LOT_hdlc_write( *data_ptr++ ^ LOT_HDLC_ESCAPE_XOR_VALUE );
        }
        else
        {
            LOT_hdlc_write( *data_ptr++ );
        }
    }
    uint8_t temp = LOT_crc_8_ccitt_check( data, _count );
    if ( ( temp == LOT_HDLC_START_END_FLAG ) || ( temp == LOT_HDLC_ESCAPE_FLAG ) )
    {
        LOT_hdlc_write( LOT_HDLC_ESCAPE_FLAG );
        LOT_hdlc_write( temp ^ LOT_HDLC_ESCAPE_XOR_VALUE );
    }
    else
    {
        LOT_hdlc_write( temp );
    }
    LOT_hdlc_write( LOT_HDLC_START_END_FLAG );
}

/**
 * @brief 수신받은 데이터에서 HDLC 프레임에 맞춰 가변 데이터 추출
 * @warning 루프 내에서 반복적으로 실행되어야 함, 버퍼가 찬 상태에서 새로운 프레임이 들어오면 오래된 프레임 버림
 */
void LOT_hdlc_recieve( void )
{
    while ( LOT_hdlc_internal_available() )
    {
        uint8_t data = LOT_hdlc_read();
        switch ( data )
        {
            case LOT_HDLC_START_END_FLAG:
                if ( LOT_hdlc_old_data == LOT_HDLC_START_END_FLAG )
                {
                    LOT_hdlc_data_count[LOT_hdlc_buf_head] = 0;
                    LOT_hdlc_status                        = LOT_HDLC_START;
                }
                else
                {
                    /// 데이터 검사
                    if ( LOT_crc_8_ccitt_check( LOT_hdlc_buf[LOT_hdlc_buf_head],
                                                LOT_hdlc_data_count[LOT_hdlc_buf_head]-- )
                         == 0 )
                    {
                        LOT_hdlc_buf_head = ( LOT_hdlc_buf_head + 1 ) % LOT_HDLC_BUF_SIZE;
                    }
                    LOT_hdlc_status = LOT_HDLC_STOP;
                }
                break;
            case LOT_HDLC_ESCAPE_FLAG:
                LOT_hdlc_status = LOT_HDLC_ESCAPE;
                break;
            default:
                /// 데이터량이 최대량 보다 많으면 데이터 손실이므로 버림
                if ( LOT_hdlc_data_count[LOT_hdlc_buf_head] < LOT_HDLC_DATA_SIZE + 1 )
                {
                    /// ESCAPE 처리
                    if ( LOT_hdlc_status == LOT_HDLC_ESCAPE )
                    {
                        LOT_hdlc_buf[LOT_hdlc_buf_head][LOT_hdlc_data_count[LOT_hdlc_buf_head]++]
                            = data ^ LOT_HDLC_ESCAPE_XOR_VALUE;
                        LOT_hdlc_status = LOT_HDLC_START;
                    }
                    else if ( LOT_hdlc_status == LOT_HDLC_START )
                    {
                        LOT_hdlc_buf[LOT_hdlc_buf_head][LOT_hdlc_data_count[LOT_hdlc_buf_head]++] = data;
                    }
                }
                break;
        }
        LOT_hdlc_old_data = data;
    }
}

/**
 * @brief 저장된 프레임 수 확인
 * @return uint8_t 저장된 프레임 수 반환
 */
uint8_t LOT_hdlc_available( void )
{
    return ( LOT_HDLC_BUF_SIZE + LOT_hdlc_buf_head - LOT_hdlc_buf_tail ) % LOT_HDLC_BUF_SIZE;
}

/**
 * @brief 저장된 프레임 중 가장 오래된 프레임의 가변 데이터 반환
 * @param uint8_t **data 1차원 데이터 배열의 주소를 입력, 추출된 데이터 반환
 * @param uint8_t *count 데이터 수에 대한 변수의 주소 입력, 데이터 수 반환
 */
void LOT_hdlc_get_buf( uint8_t **data, uint8_t *count )
{
    *data             = LOT_hdlc_buf[LOT_hdlc_buf_tail];
    *count            = LOT_hdlc_data_count[LOT_hdlc_buf_tail];
    LOT_hdlc_buf_tail = ( LOT_hdlc_buf_tail + 1 ) % LOT_HDLC_BUF_SIZE;
}
