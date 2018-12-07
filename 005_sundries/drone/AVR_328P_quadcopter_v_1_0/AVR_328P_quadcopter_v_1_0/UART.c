/********************************************************************
 *  filename:   UART.c
 *  created:    2017/09/18  15:57
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

#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#if defined(__AVR_ATmega328P__)
#else
    #error "unsupported device"
#endif


#define UART0_RX_BUF_SIZE 64 //must be power of 2
#define UART0_TX_BUF_SIZE 64
#define UART1_RX_BUF_SIZE 64
#define UART1_TX_BUF_SIZE 64
#define UART2_RX_BUF_SIZE 64
#define UART2_TX_BUF_SIZE 64
#define UART3_RX_BUF_SIZE 64
#define UART3_TX_BUF_SIZE 64


#ifdef UART0_ENABLE
    static volatile uint8_t uart0_tx_buf_head;
    static volatile uint8_t uart0_tx_buf_tail;
    static volatile uint8_t uart0_tx_buf[UART0_TX_BUF_SIZE];
    static volatile uint8_t uart0_rx_buf_head;
    static volatile uint8_t uart0_rx_buf_tail;
    static volatile uint8_t uart0_rx_buf[UART0_RX_BUF_SIZE];
#endif // UART0_ENABLE


#define UART_WRITE_INT \
uint8_t i = 0; \
while (1) \
{ \
    Q = data/10; \
    R = data - (Q * 10); \
    data = Q; \
    buf[i] = 0x30 + R; \
    if(Q == 0)break; \
    i++; \
} \
do \
{ \
    uart0_write(buf[i]); \
} \
while(i--)

#ifdef UART0_ENABLE

ISR(USART_RX_vect){
    uart0_rx_buf[uart0_rx_buf_head] = UDR0;
    // it suppose head has no data
    uart0_rx_buf_head = (uart0_rx_buf_head + 1) % UART0_RX_BUF_SIZE;
    // if head ate tail, discard a oldest data
    if(uart0_rx_buf_head == uart0_rx_buf_tail) uart0_rx_buf_tail = (uart0_rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
}

ISR(USART_UDRE_vect){
    UDR0 = uart0_tx_buf[uart0_tx_buf_tail];
    uart0_tx_buf_tail = (uart0_tx_buf_tail + 1) % UART0_TX_BUF_SIZE;
    UCSR0A |= (1<<TXC0);
    // if transmission was completed, clear UDRIE0
    if(uart0_tx_buf_head == uart0_tx_buf_tail) UCSR0B &= ~(1<<UDRIE0);
}

void uart0_begin(uint32_t baud_rate)
{

    //8bit, 1 stop bit, no parity

    UCSR0A = 0;
    UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
    uint16_t ubrr = (F_CPU/baud_rate/16) - 1;
    UBRR0H = ubrr >> 8;
    UBRR0L = ubrr;

    sei();
    uart0_rx_buf_head = 0;
    uart0_rx_buf_tail = 0;
    uart0_tx_buf_head = 0;
    uart0_tx_buf_tail = 0;
}

uint8_t uart0_read(void)
{
    if(uart0_rx_buf_head == uart0_rx_buf_tail) return 0;
    else{
        uint8_t buf = uart0_rx_buf[uart0_rx_buf_tail];
        uart0_rx_buf_tail = (uart0_rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
        return buf;
    }
}

void uart0_write(uint8_t data)
{
    if((uart0_tx_buf_head == uart0_tx_buf_tail) && (UCSR0A & (1<<UDRE0))){
        UDR0 = data;
        UCSR0A |= (1<<TXC0);
        return;
    }
    //if it can not transmit directly, it use interrupt
    uart0_tx_buf[uart0_tx_buf_head] = data;
    // it suppose head has no data
    uart0_tx_buf_head = (uart0_tx_buf_head + 1) % UART0_TX_BUF_SIZE;
    // if head ate tail, wait for a oldest data to be transmitted
    while (uart0_tx_buf_head == uart0_tx_buf_tail){}
    UCSR0B |= 1<<UDRIE0;
}

uint8_t uart0_available(void)
{
    return (UART0_RX_BUF_SIZE + uart0_rx_buf_head - uart0_rx_buf_tail) % UART0_RX_BUF_SIZE;
}

void uart0_flush(void){
    uart0_rx_buf_head = 0;
    uart0_rx_buf_tail = 0;
    uart0_rx_buf[0] = 0;
}

void uart0_write_str(const char *str)
{
    while (*str)
    {
        uart0_write(*str++);
    }
}

void uart0_write_ui16(uint16_t data)
{
    char buf[5]; //0~65535
    uint16_t Q,R;
    UART_WRITE_INT;
}

void uart0_write_i16(int16_t data)
{
    if (data<0){
        uart0_write('-');
        data=-data;
    }
    uart0_write_ui16(data);
}
#endif // UART0_ENABLE

#ifdef UART1_ENABLE
#endif // UART1_ENABLE

#ifdef UART2_ENABLE
#endif // UART2_ENABLE

#ifdef UART3_ENABLE
#endif // UART3_ENABLE