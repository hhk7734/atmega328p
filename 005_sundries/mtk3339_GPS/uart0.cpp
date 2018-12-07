/***********************************************************************************
 * filename:  uart0.cpp
 *
 * Created:   2018/01/24
 * edited:    2018/01/26
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "uart0.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//
// Static Variables initialization
// type uart0_::variables = init;
//
volatile uint8_t uart0_::uart0_tx_buf_head = 0;
volatile uint8_t uart0_::uart0_tx_buf_tail = 0;
volatile uint8_t uart0_::uart0_tx_buf[UART0_TX_BUF_SIZE] = {0};
volatile uint8_t uart0_::uart0_rx_buf_head = 0;
volatile uint8_t uart0_::uart0_rx_buf_tail = 0;
volatile uint8_t uart0_::uart0_rx_buf[UART0_RX_BUF_SIZE] = {0};

//
// Constructor & Destructor
//
uart0_::uart0_()
{
    // variables initialization
}
uart0_::~uart0_() {}

//
// Public
//
void uart0_::begin(const uint32_t &baud_rate)
{
    UCSR0A = 0;
    UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
    uint16_t ubrr = (F_CPU/baud_rate/16) - 1;
    UBRR0H = ubrr >> 8;
    UBRR0L = ubrr;

    sei();
}

uint8_t uart0_::read(void)
{
    if(uart0_rx_buf_head == uart0_rx_buf_tail) return 0;
    else{
        uint8_t buf = uart0_rx_buf[uart0_rx_buf_tail];
        uart0_rx_buf_tail = (uart0_rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
        return buf;
    }
}

void uart0_::write(const uint8_t &data)
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

void uart0_::write(const char *str)
{
    while (*str)
    {
        write(*str++);
    }
}

uint8_t uart0_::available(void)
{
    return (UART0_RX_BUF_SIZE + uart0_rx_buf_head - uart0_rx_buf_tail) % UART0_RX_BUF_SIZE;
}

uint8_t uart0_::tx_free(void)
{
    return uart0_tx_buf_head == uart0_tx_buf_tail;
}

void uart0_::flush(void)
{
    uart0_rx_buf_head = 0;
    uart0_rx_buf_tail = 0;
    uart0_rx_buf[0] = 0;
}

void uart0_::rx_isr(void)
{
    uart0_rx_buf[uart0_rx_buf_head] = UDR0;
    // it suppose head has no data
    uart0_rx_buf_head = (uart0_rx_buf_head + 1) % UART0_RX_BUF_SIZE;
    // if head ate tail, discard a oldest data
    if(uart0_rx_buf_head == uart0_rx_buf_tail) uart0_rx_buf_tail = (uart0_rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
}

void uart0_::udre_isr(void)
{
    UDR0 = uart0_tx_buf[uart0_tx_buf_tail];
    uart0_tx_buf_tail = (uart0_tx_buf_tail + 1) % UART0_TX_BUF_SIZE;
    UCSR0A |= (1<<TXC0);
    // if transmission was completed, clear UDRIE0
    if(uart0_tx_buf_head == uart0_tx_buf_tail) UCSR0B &= ~(1<<UDRIE0);
}

//
//
//

uart0_ uart0 = uart0_();

#if defined(USART_RX_vect)
ISR(USART_RX_vect)
#elif defined(USART0_RX_vect)
ISR(USART0_RX_vect)
#else
#error "USART0_RX_vect is undefined"
#endif
{
    uart0.rx_isr();
}
#if defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#elif defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect)
#else
#error "USART0_UDRE_vect is undefined"
#endif
{
    uart0.udre_isr();
}