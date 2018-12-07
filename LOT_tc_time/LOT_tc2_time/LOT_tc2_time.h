/**
 * @file LOT_tc2_time.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief TC2, 전원 인가 후 시간
 */

#ifndef _LOT_TC2_TIME_H_
#define _LOT_TC2_TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if !defined( __AVR_ATmega328P__ )
#    error "maybe unsupported device"
#endif // __AVR_ATmega328P__

#ifndef F_CPU
#    error "F_CPU is not defined"
#endif // F_CPU

void     LOT_tc2_time_setup( void );
uint32_t LOT_micros( void );
uint32_t LOT_millis( void );

#ifdef __cplusplus
}
#endif
#endif // _LOT_TC2_TIME_H_