/**
 * @file LOT_tc0_time.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief
 */

#ifndef _LOT_TC0_TIME_H_
#define _LOT_TC0_TIME_H_

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

void tc0_time_init( void );

uint32_t micros( void );

uint32_t millis( void );

void delay_us( void );

void delay_ms( void );

#ifdef __cplusplus
}
#endif
#endif    // _LOT_TC0_TIME_H_