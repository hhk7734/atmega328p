/********************************************************************
 *  filename:   UART.h
 *  created:    2017/09/18  15:56
 *
 *  Hyeon-ki, Hong
 *  Hanyang Univ., Korea
 *  hhk7734@gmail.com
 *  https://hbrotherswiki.blogspot.kr/
 *  https://www.youtube.com/channel/UCyYbIYEub_ZksB185PunUyQ
 *
 *  purpose:
 *  Universal Asynchronous Receiver Transceiver
 ********************************************************************/

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#if !defined(UART0_ENABLE) && !defined(UART_ENABLE)
    #define UART0_ENABLE
#endif

#if !defined(UART1_ENABLE) || !defined(UART2_ENABLE) || !defined(UART3_ENABLE)
    #warning "if u need UART1~3, define UART1~3_ENABLE"
#endif

#ifndef F_CPU
    #warning "F_CPU is not defined, default 16MHz"
    #define F_CPU 16000000UL
#endif

#define uart_begin(baud_rate)   uart0_begin(baud_rate)
#define uart_read()             uart0_read()
#define uart_write(data)        uart0_write(data)
#define uart_available()        uart0_available()
#define uart_flush()            uart0_flush()

#ifdef UART0_ENABLE
    void    uart0_begin(uint32_t baud_rate);
    uint8_t uart0_read(void);
    void    uart0_write(uint8_t data);
    uint8_t uart0_available(void);
    void    uart0_flush(void);

    void    uart0_write_str(const char *str);
    void    uart0_write_ui16(uint16_t data);
    void    uart0_write_i16(int16_t data);
#endif // UART0_ENABLE

#ifdef UART1_ENABLE
#endif // UART1_ENABLE

#ifdef UART2_ENABLE
#endif // UART2_ENABLE

#ifdef UART3_ENABLE
#endif // UART3_ENABLE
#endif /* UART_H_ */
