/***********************************************************************************
 * filename:  uart1.cpp
 *
 * Created:   2018/01/24
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "uart1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//
// Static Variables initialization
// type uart1_::variables = init;
//
volatile uint8_t uart1_::uart1_tx_buf_head = 0;
volatile uint8_t uart1_::uart1_tx_buf_tail = 0;
volatile uint8_t uart1_::uart1_tx_buf[UART1_TX_BUF_SIZE] = {0};
volatile uint8_t uart1_::uart1_rx_buf_head = 0;
volatile uint8_t uart1_::uart1_rx_buf_tail = 0;
volatile uint8_t uart1_::uart1_rx_buf[UART1_RX_BUF_SIZE] = {0};

//
// Constructor & Destructor
//
uart1_::uart1_()
{
    // variables initialization
}
uart1_::~uart1_() {}

//
// Public
//
void uart1_::begin(const uint32_t &baud_rate)
{
    UCSR1A = 0;
    UCSR1B = (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
    UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
    uint16_t ubrr = (F_CPU/baud_rate/16) - 1;
    UBRR1H = ubrr >> 8;
    UBRR1L = ubrr;

    sei();
}

uint8_t uart1_::read(void)
{
    if(uart1_rx_buf_head == uart1_rx_buf_tail) return 0;
    else{
        uint8_t buf = uart1_rx_buf[uart1_rx_buf_tail];
        uart1_rx_buf_tail = (uart1_rx_buf_tail + 1) % UART1_RX_BUF_SIZE;
        return buf;
    }
}

void uart1_::write(const uint8_t &data)
{
    if((uart1_tx_buf_head == uart1_tx_buf_tail) && (UCSR1A & (1<<UDRE1))){
        UDR1 = data;
        UCSR1A |= (1<<TXC1);
        return;
    }
    //if it can not transmit directly, it use interrupt
    uart1_tx_buf[uart1_tx_buf_head] = data;
    // it suppose head has no data
    uart1_tx_buf_head = (uart1_tx_buf_head + 1) % UART1_TX_BUF_SIZE;
    // if head ate tail, wait for a oldest data to be transmitted
    while (uart1_tx_buf_head == uart1_tx_buf_tail){}
    UCSR1B |= 1<<UDRIE1;
}

void uart1_::write(const char *str)
{
    while (*str)
    {
        write(*str++);
    }
}

uint8_t uart1_::available(void)
{
    return (UART1_RX_BUF_SIZE + uart1_rx_buf_head - uart1_rx_buf_tail) % UART1_RX_BUF_SIZE;
}

void uart1_::flush(void)
{
    uart1_rx_buf_head = 0;
    uart1_rx_buf_tail = 0;
    uart1_rx_buf[0] = 0;
}

void uart1_::rx_isr(void)
{
    uart1_rx_buf[uart1_rx_buf_head] = UDR1;
    // it suppose head has no data
    uart1_rx_buf_head = (uart1_rx_buf_head + 1) % UART1_RX_BUF_SIZE;
    // if head ate tail, discard a oldest data
    if(uart1_rx_buf_head == uart1_rx_buf_tail) uart1_rx_buf_tail = (uart1_rx_buf_tail + 1) % UART1_RX_BUF_SIZE;
}

void uart1_::udre_isr(void)
{
    UDR1 = uart1_tx_buf[uart1_tx_buf_tail];
    uart1_tx_buf_tail = (uart1_tx_buf_tail + 1) % UART1_TX_BUF_SIZE;
    UCSR1A |= (1<<TXC1);
    // if transmission was completed, clear UDRIE1
    if(uart1_tx_buf_head == uart1_tx_buf_tail) UCSR1B &= ~(1<<UDRIE1);
}

//
//
//

uart1_ uart1 = uart1_();
#if defined(USART1_RX_vect)
ISR(USART1_RX_vect)
#else
#error "USART1_RX_vect is undefined"
#endif
{
    uart1.rx_isr();
}
#if defined(USART1_UDRE_vect)
ISR(USART1_UDRE_vect)
#else
#error "USART1_UDRE_vect is undefined"
#endif
{
    uart1.udre_isr();
}