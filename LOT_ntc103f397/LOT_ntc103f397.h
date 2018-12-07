/**
 * @file LOT_ntc103f397.h
 * @author Hyeon-ki, Hong (hhk7734@gmail.com)
 * @brief NTC 103 F 397 써미스터
 */

#ifndef _LOT_NTC103F397_H_
#define _LOT_NTC103F397_H_
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name 사용자 정의 매크로
 * @warning 사용하기 전 필수적으로 확인
 * @{
 */
/// 0 ~ 15, 값이 커질 수록 LPF의 cutoff freq가 작아짐
#define LOT_NTC_LPF_FACTOR 7
/// @}

/// 1/T0 - (1/B)*ln(R0)
#define LOT_NTC_STEINHART_HART_EQU_A 0.00277402f
/// 1/B
#define LOT_NTC_STEINHART_HART_EQU_B 0.000251889f
#define LOT_NTC_STEINHART_HART_EQU_C 0.000000254f

float LOT_ntc_temperature( float kohm );

#ifdef __cplusplus
}
#endif
#endif // _LOT_NTC103F397_H_
