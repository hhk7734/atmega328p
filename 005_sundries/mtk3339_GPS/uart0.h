/***********************************************************************************
 * filename:  uart0.h
 *
 * Created:   2018/01/24
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _UART0_H_
#define _UART0_H_

#include <inttypes.h>

#if defined(__AVR_ATmega128A__) || defined(__AVR_ATmega328P__)
#else
#error "maybe unsupported device"
#endif

#ifndef F_CPU
#warning "F_CPU is not defined, default 16MHz"
#define F_CPU 16000000UL
#endif

#define UART0_RX_BUF_SIZE 256 //must be power of 2
#define UART0_TX_BUF_SIZE 128



class uart0_
{
public :
    uart0_();
    ~uart0_();
    void    begin(const uint32_t &baud_rate);
    uint8_t read(void);
    void    write(const uint8_t &data);
    void    write(const char *str);
    uint8_t available(void);
    uint8_t tx_free(void);
    void    flush(void);
    void    rx_isr(void);
    void    udre_isr(void);

protected :

private :
    static volatile uint8_t uart0_tx_buf_head;
    static volatile uint8_t uart0_tx_buf_tail;
    static volatile uint8_t uart0_tx_buf[UART0_TX_BUF_SIZE];
    static volatile uint8_t uart0_rx_buf_head;
    static volatile uint8_t uart0_rx_buf_tail;
    static volatile uint8_t uart0_rx_buf[UART0_RX_BUF_SIZE];
}; // uart0_

extern uart0_ uart0;
#endif // _UART0_H_