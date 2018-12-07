/***********************************************************************************
 * filename:  mtk3339.cpp
 *
 * Created:   2018/01/29
 * edited:    2018/02/01
 *
 * Hyeon-ki, Hong
 * Hanyang Univ., Korea
 * hhk7734@gamil.com
 * https://hbrotherswiki.blogspot.kr/
 **********************************************************************************/

#include "mtk3339.h"

#include <avr/io.h>
#include <util/delay.h>
//#include ""

//
// Static Variables initialization
// type mtk3339_::variables = init;
//
uint8_t  mtk3339_::parsing_step = 0;
uint8_t  mtk3339_::checksum = 0;
uint8_t  mtk3339_::offset = 0;
uint8_t  mtk3339_::uncheck_checksum = 0;
char     mtk3339_::string[15] = {0};

uint8_t  mtk3339_::temp_gga_valid = 0;
int32_t  mtk3339_::temp_latitude = 0;
int32_t  mtk3339_::temp_longitude = 0;
uint16_t mtk3339_::temp_altitude = 0;
uint8_t  mtk3339_::temp_num_satellite = 0;
uint8_t  mtk3339_::temp_rmc_valid = 0;
uint16_t mtk3339_::temp_speed = 0;
int16_t  mtk3339_::temp_gps_direction = 0;

uint8_t  mtk3339_::gga_valid = 0;
int32_t  mtk3339_::latitude = 0;
int32_t  mtk3339_::longitude = 0;
uint16_t mtk3339_::altitude = 0;
uint8_t  mtk3339_::num_satellite = 0;
uint8_t  mtk3339_::rmc_valid = 0;
uint16_t mtk3339_::speed = 0;
int16_t  mtk3339_::gps_direction = 0;

//
// Constructor & Destructor
//
mtk3339_::mtk3339_()
{
    // variables initialization
}
mtk3339_::~mtk3339_() {}

//
// Public
//
void mtk3339_::begin(const uint32_t &baud_rate)
{
    uint32_t init_speed[5] = {9600,19200,38400,57600,115200};
    uint8_t _check;
    for (uint8_t i = 0 ; i < 5 ; ++i)
    {
        uart0_::begin(init_speed[i]);
        switch (baud_rate)
        {
            case 9600 :
            write(PMTK_SET_BAUD_9600);
            _check = 0;
            break;
            case 19200 :
            write(PMTK_SET_BAUD_19200);
            _check = 1;
            break;
            case 38400 :
            write(PMTK_SET_BAUD_38400);
            _check = 2;
            break;
            default :
            write(PMTK_SET_BAUD_57600);
            _check = 3;
            break;
        }
        while(!tx_free()) _delay_ms(100);
    }
    uart0_::begin(init_speed[_check]);

    write(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    while(!tx_free()) _delay_ms(100);

    write(PMTK_SET_NMEA_UPDATE_10HZ);
    while(!tx_free()) _delay_ms(100);

    write(PMTK_SET_NAV_THRESHOLD_OFF);
    while(!tx_free()) _delay_ms(100);

    write(PMTK_API_SET_DGPS_WAAS);
    while(!tx_free()) _delay_ms(100);

    write(PMTK_API_SET_SBAS_ENABLED);
    while(!tx_free()) _delay_ms(100);

    write(PMTK_API_SET_SBAS_TEST_MODE);
    while(!tx_free()) _delay_ms(100);
}

void mtk3339_::update(void)
{
    uint8_t valid = parse();
    if(valid && temp_gga_valid)
    {
		gga_valid = temp_gga_valid;
        latitude = temp_latitude;
        longitude = temp_longitude;
        altitude = temp_altitude;
        num_satellite = temp_num_satellite;
    }
    if (valid && temp_rmc_valid)
    {
		rmc_valid = temp_rmc_valid;
        speed = temp_speed;
        gps_direction = temp_gps_direction;
    }
}

uint8_t mtk3339_::get_gga_valid(void)
{
    return gga_valid;
}

int32_t mtk3339_::get_latitude(void)
{
    return latitude;
}

int32_t mtk3339_::get_longitude(void)
{
    return longitude;
}

uint16_t mtk3339_::get_altitude(void)
{
    return altitude;
}

uint8_t mtk3339_::get_num_satellite(void)
{
    return num_satellite;
}

uint8_t mtk3339_::get_rmc_valid(void)
{
    return rmc_valid;
}

uint16_t mtk3339_::get_speed(void)
{
    return speed;
}

int16_t mtk3339_::get_gps_direction(void)
{
    return gps_direction;
}

//
// Protected
//


//
// Private
//
uint32_t mtk3339_::dmm_to_degree(char *str)
{
    char *digit, *index;
    uint8_t  ddd = 0;
    uint8_t  mm = 0;
    uint16_t dot_mmmm = 0;

    // scan '.'
    for(digit = str; *digit != '.'; ++digit);
    index = str;

    while(digit - index > 2)
    {
        ddd *= 10;
        ddd += *index - '0';
        ++index;
    }

    while(digit > index)
    {
        mm *= 10;
        mm += *index - '0';
        ++index;
    }

    ++index; // skip '.'

    while(*index != 0)
    {
        dot_mmmm *= 10;
        dot_mmmm += *index - '0';
        ++index;
    }

    // xxx.xxxxxxx ==> x,xxx,xxx,xxx  2^31 == 2,147,483,648
    return ddd * 10000000UL + (mm * 1000000UL + dot_mmmm * 100UL) / 6;
}


uint8_t mtk3339_::hex_to_dec(uint8_t data)
{
    data -= '0';
    if(data > 9) data -= 7;
    return data;
}

uint16_t mtk3339_::string_to_int_mul(char *str, const uint8_t &ten_power)
{
    uint16_t temp = 0;
    for (uint8_t i = 0; str[i] != 0 ; ++i)
    {
        if(str[i] =='.')
        {
            ++i;
            if (ten_power == 0) break;
            else str[i+ten_power] = 0;
        }
        temp *= 10;
        temp += str[i] - '0';
    }

    return temp;
}

uint8_t mtk3339_::parse(void)
{
    uint8_t valid = 0;
    uint8_t i = available();
    if (i > MAX_PARSING_DATA)
    {
        i = MAX_PARSING_DATA;
    }

    for (;i;--i)
    {
        uint8_t data = read();
        if(data == '$')
        {
            parsing_step = 1;
            checksum = 0;
            offset = 0;
        }
        else if(data == ',' || data == '*')
        {
            string[offset] = 0;
            switch(parsing_step)
            {
                case 1:
                if(string[2] == 'G' && string[3] == 'G' && string[4] == 'A') parsing_step = FRAME_GGA;
                else if(string[2] == 'R' && string[3] == 'M' && string[4] == 'C') parsing_step = FRAME_RMC;
                break;

                case 13:
                // GPGGA Latitude
                // ddmm.mmmm ==> degree
                temp_latitude = dmm_to_degree(string);
                break;

                case 14:
                // GPGGA N/S Indicator N == +, S == -
                if(string[0] == 'S') temp_latitude = -temp_latitude;
                break;

                case 15:
                // GPGGA Longitude
                // ddmm.mmmm ==> degree
                temp_longitude = dmm_to_degree(string);
                break;

                case 16:
                // GPGGA E/W Indicator E == +, W == -
                if(string[0] == 'W') temp_longitude = -temp_longitude;
                break;

                case 17:
                // GPGGA Status
                // '0' == Invalid, '1' == Valid SPS '2' == Valid DGPS '3' == Valid PPS
                temp_gga_valid = string[0] - '0';
                break;

                case 18:
                // GPGGA Satellites Used
                temp_num_satellite = string_to_int_mul(string);
                break;

                case 20:
                // GPGGA Altitude
                // xx.x meter * 10 ==> xxx * 10 ==> xxx0 cm
                temp_altitude = string_to_int_mul(string,1) * 10;
                break;

                case 33:
                // GPRMC Status
                // A = Active, V = Void
                if (string[0] == 'A') temp_rmc_valid = 1;
                else temp_rmc_valid = 0;
                break;

                case 38:
                // GPRMC Speed over ground
                // xx.xx Knots * 10 ==> xxx(.x) * 5.144 ==> cm/s  (5.144 == 5267 / 1024)
                //
                temp_speed = ((uint32_t)string_to_int_mul(string,1) * 5267)>> 10;
                break;

                case 39:
                // GPRMC Course over ground
                // 0~360 degree clockwise (True North == 0)
                // *10 == degree*10 (0 ~ 3600)
                // -1800 ~ 1800
                temp_gps_direction = string_to_int_mul(string,1);
                if (temp_gps_direction >1800) temp_gps_direction -= 3600;
                break;
            }
            offset = 0;
            ++parsing_step;
            if(data == '*') uncheck_checksum = 1;
            else
            {
                checksum ^= data;
            }
        }
        else if (data == '\r' || data == '\n')
        {
            // between '$' and '*' XOR
            if(uncheck_checksum)
            {
                uint8_t temp = hex_to_dec(string[0]);
                temp <<= 4;
                temp += hex_to_dec(string[1]);
                if (temp == checksum) valid = 1;
            }
            uncheck_checksum = 0;
        }
        else
        {
            string[offset++] = data;
            if(!uncheck_checksum)checksum ^= data;
        }
    }
    return valid;
}

//
// mtk3339_ [Object name] = mtk3339_();
//
mtk3339_ mtk3339 = mtk3339_();