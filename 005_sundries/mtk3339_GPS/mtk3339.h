/***********************************************************************************
 * filename:  mtk3339.h
 *
 * Created:   2018/01/29
 * edited:    2018/02/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#ifndef _MTK3339_H_
#define _MTK3339_H_

#include <inttypes.h>

#include "uart0.h"

#define MAX_PARSING_DATA 15

//#define
#define PMTK_ACK "$PMTK001,[command],[flag]*[checksum]"
// 0 invalid
// 1 unsupported
// 2 valid, but action failed
// 3 valid, action succeeded

#define PMTK_CMD_HOT_START       "$PMTK101*32\r\n"
#define PMTK_CMD_WARM_START      "$PMTK102*31\r\n"
#define PMTK_CMD_COLD_START      "$PMTK103*30\r\n"
#define PMTK_CMD_FULL_COLD_START "$PMTK104*37\r\n" // reset the receiver to the factory status

#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B\r\n"
#define PMTK_SET_NMEA_UPDATE_1HZ             "$PMTK220,1000*1F\r\n"
#define PMTK_SET_NMEA_UPDATE_5HZ             "$PMTK220,200*2C\r\n"
#define PMTK_SET_NMEA_UPDATE_10HZ            "$PMTK220,100*2F\r\n"

#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F\r\n" // if device is 16MHz AVR, don't use it
#define PMTK_SET_BAUD_57600  "$PMTK251,57600*2C\r\n"
#define PMTK_SET_BAUD_38400  "$PMTK251,38400*27\r\n"
#define PMTK_SET_BAUD_19200  "$PMTK251,19200*22\r\n"
#define PMTK_SET_BAUD_9600   "$PMTK251,9600*17\r\n"

#define PMTK_API_SET_DGPS_DISABLE "$PMTK301,0*2C\r\n"
#define PMTK_API_SET_DGPS_RTCM    "$PMTK301,1*2D\r\n" // if in Korea, need another device
#define PMTK_API_SET_DGPS_WAAS    "$PMTK301,2*2E\r\n" // in USA, maybe in Korea

#define PMTK_API_SET_SBAS_DISABLE "$PMTK313,0*2F\r\n"
#define PMTK_API_SET_SBAS_ENABLED "$PMTK313,1*2E\r\n" // MSAS in Japan, maybe in Korea

#define PMTK_API_SET_SBAS_TEST_MODE      "$PMTK319,0*25\r\n"
#define PMTK_API_SET_SBAS_INTEGRITY_MODE "$PMTK319,1*24\r\n"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA  "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"

#define PMTK_SET_NAV_THRESHOLD_OFF "$PMTK386,0*23\r\n"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31\r\n"
// return PMTK705,[ReleaseStr],[Build_ID],[Internal_USE_1,..]

#define PMTK_CMD_STANDBY_MODE "$PMTK161,0*28\r\n"
#define PMTK_AWAKE            "$PMTK010,002*2D\r\n"

#define FRAME_GGA  11
#define FRAME_RMC  31


// 256 uart0_ RX buffer is needed for GPS communication
// uart0_, add tx_free()
class mtk3339_ : public uart0_
{

public :
    mtk3339_();
    ~mtk3339_();
    void begin(const uint32_t &baud_rate);
    void update(void);
	
	uint8_t  get_gga_valid(void);
    int32_t  get_latitude(void);
    int32_t  get_longitude(void);
    uint16_t get_altitude(void);
    uint8_t  get_num_satellite(void);
	uint8_t  get_rmc_valid(void);
    uint16_t get_speed(void);
    int16_t  get_gps_direction(void);
protected :

private :
    static uint8_t parsing_step;
    static uint8_t checksum;
    static uint8_t offset;
    static uint8_t uncheck_checksum;
    static char    string[15];

	static uint8_t  temp_gga_valid;
    static int32_t  temp_latitude;
    static int32_t  temp_longitude;
    static uint16_t temp_altitude;
    static uint8_t  temp_num_satellite;
    static uint8_t  temp_rmc_valid;
    static uint16_t temp_speed;
    static int16_t  temp_gps_direction;

	static uint8_t  gga_valid;
    static int32_t  latitude;
    static int32_t  longitude;
    static uint16_t altitude;
    static uint8_t  num_satellite;
	static uint8_t  rmc_valid;
    static uint16_t speed;
    static int16_t  gps_direction;

    uint32_t dmm_to_degree(char *str);
    uint8_t  hex_to_dec(uint8_t data);
    uint16_t string_to_int_mul(char *str, const uint8_t &ten_power = 0);
    uint8_t  parse(void);

}; // mtk3339_

extern mtk3339_ mtk3339;
#endif // _MTK3339_H_