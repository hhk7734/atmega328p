/**
 * @file LOT_tc0_time.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief TC0, 전원 인가 후 시간
 */

#ifndef _LOT_TC0_TIME_H_
#define _LOT_TC0_TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if !defined( __AVR_ATmega128__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega128__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

void     LOT_tc0_time_setup( void );
uint32_t LOT_micros( void );
uint32_t LOT_millis( void );

#ifdef __cplusplus
}
#endif
#endif // _LOT_TC0_TIME_H_