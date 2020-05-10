/*
 * MIT License
 * Copyright (c) 2018 Hyeonki Hong <hhk7734@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "uart0.h"

#include <avr/interrupt.h>

volatile static uint8_t tx_buf_head               = 0;
volatile static uint8_t tx_buf_tail               = 0;
volatile static uint8_t tx_buf[UART0_TX_BUF_SIZE] = {0};
volatile static uint8_t rx_buf_head               = 0;
volatile static uint8_t rx_buf_tail               = 0;
volatile static uint8_t rx_buf[UART0_RX_BUF_SIZE] = {0};

void uart0_init_all(const uint32_t baud_rate,
                    const uint8_t  data_bits,
                    const uint8_t  parity,
                    const uint8_t  stop_bits) {
    UCSR0A = _BV(U2X0);

    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);

    if(data_bits < 9) {
        UCSR0C = (data_bits - 5) << 1;
    } else {
        /// @todo 9bit 지원
        UCSR0C = (8 - 5) << 1;
    }

    switch(parity) {
    // odd parity
    case 1: UCSR0C |= _BV(UPM00);
    // even parity
    case 2: UCSR0C |= _BV(UPM01); break;
    }

    if(stop_bits == 2) { UCSR0C |= _BV(USBS0); }

    // ( F_CPU / baud_rate / 8 ) - 1 을 계산한 뒤 소수점 첫째 자리에서 반올림
    // 해야하지만 부동소수 -> 정수로 갈때 버림을 하기 때문에 0.5를 더한다
    uint16_t temp = ((F_CPU / baud_rate / 4) - 1) / 2;
    UBRR0H        = temp >> 8;
    UBRR0L        = temp;

    sei();
}

uint8_t uart0_read() {
    if(rx_buf_head == rx_buf_tail) return 0;
    else {
        uint8_t data = rx_buf[rx_buf_tail];
        rx_buf_tail  = (rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
        return data;
    }
}

void uart0_write(const uint8_t data) {
    if((tx_buf_head == tx_buf_tail) && (UCSR0A & _BV(UDRE0))) {
        UDR0 = data;
        UCSR0A |= _BV(TXC0);
        return;
    }
    tx_buf[tx_buf_head] = data;
    tx_buf_head         = (tx_buf_head + 1) % UART0_TX_BUF_SIZE;
    while(tx_buf_head == tx_buf_tail) {}
    UCSR0B |= _BV(UDRIE0);
}

void uart0_print_str(const char *str) {
    while(*str) { uart0_write(*str++); }
}

uint8_t uart0_available() {
    return (UART0_RX_BUF_SIZE + rx_buf_head - rx_buf_tail) % UART0_RX_BUF_SIZE;
}

void uart0_flush() {
    rx_buf_head = 0;
    rx_buf_tail = 0;
    rx_buf[0]   = 0;
}

ISR(USART_RX_vect) {
    rx_buf[rx_buf_head] = UDR0;
    rx_buf_head         = (rx_buf_head + 1) % UART0_RX_BUF_SIZE;
    if(rx_buf_head == rx_buf_tail) {
        rx_buf_tail = (rx_buf_tail + 1) % UART0_RX_BUF_SIZE;
    }
}

ISR(USART_UDRE_vect) {
    UDR0        = tx_buf[tx_buf_tail];
    tx_buf_tail = (tx_buf_tail + 1) % UART0_TX_BUF_SIZE;
    UCSR0A |= _BV(TXC0);
    if(tx_buf_head == tx_buf_tail) { UCSR0B &= ~_BV(UDRIE0); }
}