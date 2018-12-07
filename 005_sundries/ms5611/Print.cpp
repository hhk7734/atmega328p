/***********************************************************************************
* filename:  Print.cpp
*
* Created:   2018/01/26
*
* Hyeon-ki, Hong
* Hanyang Univ., Korea
* hhk7734@gamil.com
* https://hbrotherswiki.blogspot.kr/
**********************************************************************************/

#include "Print.h"

#include <math.h>

// 2^16 == 65536, x == 5
#define PRINTDEC(_data,_temp,x) \
char buf[x+1]; \
char *str = &buf[x]; \
*str = '\0'; \
do{ \
    _temp = _data/10; \
    char remainder = (_data - _temp *10) +'0'; \
    *--str = remainder; \
    _data = _temp; \
}while(_data); \
print(str)

//
// Static Variables initialization
// type Print_::variables = init;
//

//
// Public
//
void Print_::print(const char *str)
{
    while (*str)
    {
        write(*str++);
    }
}

void Print_::print(uint8_t data)
{
    uint8_t temp;
    PRINTDEC(data,temp,3);
}

void Print_::print(int8_t data)
{
    if (data < 0)
    {
        write('-');
        data = -data;
    }
    print((uint8_t)data);
}

void Print_::print(uint16_t data)
{
    uint16_t temp;
    PRINTDEC(data,temp,5);
}

void Print_::print(int16_t data)
{
    if (data < 0)
    {
        write('-');
        data = -data;
    }
    print((uint16_t)data);
}

void Print_::print(uint32_t data)
{
    uint32_t temp;
    PRINTDEC(data,temp,10);
}

void Print_::print(int32_t data)
{
    if (data < 0)
    {
        write('-');
        data = -data;
    }
    print((uint32_t)data);
}

void Print_::print(uint64_t data)
{
    uint64_t temp;
    PRINTDEC(data,temp,20);
}

void Print_::print(int64_t data)
{
    if (data < 0)
    {
        write('-');
        data = -data;
    }
    print((uint64_t)data);
}

void Print_::print(float data, int digits)
{
    if (isnan(data))  // math.h
    {
        print("nan");
        return;
    }
    if (isinf(data))
    {
        print("inf");
        return;
    }
    if (data > 4294967040.0)  // constant determined empirically
    {
        print("ovf");
        return;
    }
    if (data <-4294967040.0)  // constant determined empirically
    {
        print("ovf");
        return;
    }

    // Handle negative numbers
    if (data < 0.0)
    {
        print('-');
        data = -data;
    }

    uint32_t _int = (uint32_t)data;
    print(_int);
    print('.');

    float _dec = data - (float)_int;
    char buf[10];
    char *str = &buf[9];
    *str = '\0';

    if(digits < 5)
    {
        uint16_t temp = 1;
        for (uint8_t i=0; i<digits; ++i) temp *= 10;
        uint16_t _dec_up = (float) (_dec * temp + 0.5);

        while(digits--)
        {
            temp = _dec_up/10;
            char remainder = (_dec_up - temp *10) +'0';
            *--str = remainder;
            _dec_up = temp;
        }
        print(str);
    }
    else if(digits<10)
    {
        uint32_t temp = 1;
        for (uint8_t i=0; i<digits; ++i) temp *= 10;
        uint32_t _dec_up = (float) (_dec * temp + 0.5);

        while(digits--)
        {
            temp = _dec_up/10;
            char remainder = (_dec_up - temp *10) +'0';
            *--str = remainder;
            _dec_up = temp;
        }
        print(str);
    }
    else
    {
        // Round correctly so that print(1.999, 2) prints as "2.00"
        float rounding = 0.5;
        for (uint8_t i=0; i<digits; ++i)
        rounding /= 10.0;

        _dec += rounding;

        // Extract digits from the remainder one at a time
        while (digits--)
        {
            _dec *= 10.0;
            uint8_t toPrint = (uint8_t)(_dec);
            print(toPrint);
            _dec -= toPrint;
        }
    }

}

//
// Protected
//

//
// Private
//