/***********************************************************************************
* filename:  Print.h
*
* Created:   2018/01/26
*
* Hyeon-ki, Hong
* Hanyang Univ., Korea
* hhk7734@gamil.com
* https://hbrotherswiki.blogspot.kr/
**********************************************************************************/

#ifndef _PRINT_H_
#define _PRINT_H_

#include <inttypes.h>

class Print_
{
public :
virtual void write(const uint8_t &data) = 0;

inline void print(const char data){write(data);}
void print(const char *str);
void print(uint8_t  data);
void print(int8_t   data);
void print(uint16_t data);
void print(int16_t  data);
void print(uint32_t data);
void print(int32_t  data);
void print(uint64_t data);
void print(int64_t  data);
void print(float    data, int digits = 2);
inline void print(double data, int digits = 2){print((float)data, digits);}

inline void println(void) {print("\r\n");}
inline void println(const char data){write(data);println();}
inline void println(const char *str){print(str);println();}
inline void println(uint8_t  data)  {print(data);println();}
inline void println(int8_t   data)  {print(data);println();}
inline void println(uint16_t data)  {print(data);println();}
inline void println(int16_t  data)  {print(data);println();}
inline void println(uint32_t data)  {print(data);println();}
inline void println(int32_t  data)  {print(data);println();}
inline void println(uint64_t data)  {print(data);println();}
inline void println(int64_t  data)  {print(data);println();}
inline void println(float    data, int digits = 2){print(data,digits);println();}
inline void println(double data, int digits = 2){print(data,digits);println();}
protected :

private :

}; // Print_

#endif // _PRINT_H_