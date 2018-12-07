/**
 * @file LOT_crc_8_ccitt.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief CRC-8-CCITT, X^8 + X^2 + X + 1
 */

#ifndef _LOT_CRC_8_CCITT_H_
#define _LOT_CRC_8_CCITT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
#define _LOT_USE_CRC_8_CCITT_TABEL_
#define LOT_CRC_8_CCITT_INIT 0x00
/**
 * @}
 */

uint8_t LOT_crc_8_ccitt_check( const uint8_t *data, uint8_t count );

#ifdef __cplusplus
}
#endif
#endif // _LOT_CRC_8_CCITT_H_
