/**
 * @file LOT_ntc103f397.c
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief NTC 103 F 397 써미스터
 */

#include "LOT_ntc103f397.h"
#include "inttypes.h"
#include <math.h>

/**
 * @brief 이전 값에 새로운 값을 대입할 때, 최대 변화량을 제한
 * @param old 이전 값
 * @param new 새로운 값
 * @param interval 최대 변화량
 */
#define con( old, new, interval )          \
    ( ( new ) < ( ( old ) - ( interval ) ) \
          ? ( ( old ) - ( interval ) )     \
          : ( ( new ) > ( ( old ) + ( interval ) ) ? ( ( old ) + ( interval ) ) : ( new ) ) )

static uint8_t LOT_ntc_step      = 0;
static int32_t LOT_ntc_T_lpf     = 0;
static int32_t LOT_ntc_T_display = 0;

/**
 * @brief 써미스터의 저항값을 입력하면 온도 출력
 * @param float kohm 써미스터의 저항값, 단위 k옴
 * @return float 섭씨 온도
 */
float LOT_ntc_temperature( float kohm )
{
    float temp = log( kohm );
    float T    = LOT_NTC_STEINHART_HART_EQU_A + LOT_NTC_STEINHART_HART_EQU_B * temp
              + LOT_NTC_STEINHART_HART_EQU_C * temp * temp * temp;

    T = 1 / T - 273.15;

    /// 초기화
    if ( LOT_ntc_step < 5 )
    {
        LOT_ntc_T_display = T * 1000;
        LOT_ntc_T_lpf     = LOT_ntc_T_display << LOT_NTC_LPF_FACTOR;
        ++LOT_ntc_step;
    }

    LOT_ntc_T_lpf -= LOT_ntc_T_lpf >> LOT_NTC_LPF_FACTOR;
    LOT_ntc_T_lpf += T * 1000;

    LOT_ntc_T_display = con( LOT_ntc_T_display, LOT_ntc_T_lpf >> LOT_NTC_LPF_FACTOR, 100 );

    return ( float )( LOT_ntc_T_display ) / 1000.0;
}
