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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define UART0_RX_BUF_SIZE 64
#define UART0_TX_BUF_SIZE 64

/**
 * @param baud_rate
 * @param data_bits 5, 6, 7, 8 or 9 bits
 * @param parity 0-none, 1-odd, 2-even
 * @param stop_bits 1 or 2 bits
 */
void uart0_init_all(const uint32_t baud_rate,
                    const uint8_t  data_bits,
                    const uint8_t  parity,
                    const uint8_t  stop_bits);

inline void uart0_init(const uint32_t baud_rate) {
    uart0_init_all(baud_rate, 8, 0, 1);
}

uint8_t uart0_read();
void    uart0_write(const uint8_t data);
void    uart0_print_str(const char *str);
uint8_t uart0_available();
void    uart0_flush();

#ifdef __cplusplus
}
#endif
