/**
 * @file LOT_print.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief print 함수를 통해 숫자를 송신
 */

#ifndef _LOT_PRINT_H_
#define _LOT_PRINT_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
#include "LOT_uart0.h"
/// 문자를 송신하는 매크로 함수 설정
#define LOT_print_char(x) LOT_uart0_write(x)
/// 문자열을 송신하는 매키로 함수 설정
#define LOT_print_str(x) LOT_uart0_print_str(x)
/// @}

void LOT_print_newline(void);
void LOT_print_u8(uint8_t data);
void LOT_print_i8(int8_t data);
void LOT_print_u16(uint16_t data);
void LOT_print_i16(int16_t data);
void LOT_print_u32(uint32_t data);
void LOT_print_i32(int32_t data);
void LOT_print_u64(uint64_t data);
void LOT_print_i64(int64_t data);
void LOT_print_f(float data, uint8_t digits);

#ifdef __cplusplus
}
#endif
#endif // _LOT_PRINT_H_
