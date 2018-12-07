/***********************************************************************************
 * filename:  uart1.h
 *
 * Created:   2018/01/24
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _UART1_H_
#define _UART1_H_

#include <inttypes.h>

#include "Print.h"

#if defined(__AVR_ATmega128A__)
#else
#error "maybe unsupported device"
#endif

#ifndef F_CPU
#warning "F_CPU is not defined, default 16MHz"
#define F_CPU 16000000UL
#endif

#define UART1_RX_BUF_SIZE 64 //must be power of 2
#define UART1_TX_BUF_SIZE 64

class uart1_ : public Print_
{
public :
    uart1_();
    ~uart1_();
	
	inline void method(void) {}
	
    void    begin(const uint32_t &baud_rate);
    uint8_t read(void);
    void    write(const uint8_t &data);
    void    write(const char *str);
    uint8_t available(void);
    void    flush(void);
    void    rx_isr(void);
    void    udre_isr(void);

protected :

private :
    static volatile uint8_t uart1_tx_buf_head;
    static volatile uint8_t uart1_tx_buf_tail;
    static volatile uint8_t uart1_tx_buf[UART1_TX_BUF_SIZE];
    static volatile uint8_t uart1_rx_buf_head;
    static volatile uint8_t uart1_rx_buf_tail;
    static volatile uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
}; // uart1_

extern uart1_ uart1;
#endif // _UART1_H_