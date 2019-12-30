/*
  NMEA Parser library v1.0
  By X99

  This library is provided as-is.
  Its aim is to parse NMEA sentences from a LS20126 or any other NMEA compatible GPS.
  Why do I develop my own lib while
  there are plenty like this one, like TinyGPS? Here's the main reason: extensibility.
  Have you tried to add a new sentence to TinyGPS? I did. A real plain in the ass.
  I decided to develop my own lib, making it easy to extend. It relies heavily on
  a custom implementation of sscanf to fit my needs. Technically speaking, sscanf
  parses very wells NMEA strings, but has two main drawbacks:
  - it's ways to sophisticated for what we need here. And some datas such as latitude or
    UTC time are recognized as floats.
  - sscanf doesn't support empty fields. In NMEA strings, blank fields are common, especially
    when GPS works indoor. Default sscanf sees empty fields as errors. Mine parses them fine.

  Talking about extensibility, it's quite simple to add your own NMEA sentence:
  - declare its structure, with a default "bool isValid" and "int16_t fieldValidity" inside it.
  - declare its type in "STRINGS_TYPES"
  - create a parsing function based on existing ones
  - declare your sentence type in the "dispatch" function.
  - you're done!

  It's that simple.

  This header includes basic sentence description, at least those supported by LS20126.
*/

#pragma once

#include <Arduino.h>
#include <stdarg.h>

/*
    Arduino types
    char = 1 byte
    byte = 1 byte, unsigned by default
    int16_t = 2bytes, signed by default
    word = unsigned int
    long = 4 bytes, signed
*/

typedef unsigned char byte;

/*
    Checksum = 8bit exclusive OR of all characters, excluding '$' and '*'
*/

/*
    $PLSR,245,1
    PLSR Compass measurement report 1: calibration and acceleration.
    Rate: default 1Hz, maximum 5Hz
    Sentence ID         $PLSR,245,1
    Direction           210 (degree. Magnetic direction, 0-360, 0=north)
    Calibration status  0 (auto calibration status)
    Field intensity     1090 (0..1300, magnetic field intensity, 0microTesla to 520microTesla)
    Acceleration X      22 (-512 to 511 -> -2G to 2G)
    Acceletation Y      -15 (-512 to 511 -> -2G to 2G)
    Acceleration Z      263 (-512 to 511 -> -2G to 2G)
    Temperature         19 (Celsius)
    Mounting Mode       0
    Current Calibration 2 (non zero: valid, 0=not valid)
    Checksum            *1D
*/
struct PLSR2451 {
    bool isValid;           //this tells is struct's content is issued from a valid sentence. If not, do not use struct's datas.
    int16_t fieldValidity;      //each bit of this int16_t telle if the corresponding field is valid of not.

    int16_t direction;
    int16_t calibration_status;
    int16_t field_intensity;
    int16_t acceleration_x;
    int16_t acceleration_y;
    int16_t acceleration_z;
    int16_t temperature;
    int16_t mounting_mode;
    int16_t current_calibration;
};

/*
    $PLSR,245,2
    PLSR Compass measurement report 2: attitude. The PLSR compass measurement report 2 contains
    a set of the attitude vectors, each row of the matrix means attitude vectors and it is
    normalized with 0x1000.
    Rate: 1hz, maximum 5Hz
    Sentence ID         $PLSR,245,2,
    Xx                  -3520 (X acceleration on X axis)
    Yx                  2066 (Y acceleration on X axis)
    Zx                  340 (...)
    Xy                  -2087
    Yy                  -3516
    Zy                  -232
    Xz                  174
    Yz                  -372
    Zz                  4075
    Checksum            *11
*/
struct PLSR2452 {
    bool isValid;
    int16_t fieldValidity;

    int16_t xx;
    int16_t yx;
    int16_t zx;
    int16_t xy;
    int16_t yy;
    int16_t zy;
    int16_t xz;
    int16_t yz;
    int16_t zz;
};

/*
    $PLSR,245,7
    3D GPS Speed output (ECEF coordinate)
    Sentence ID         $PLSR,245,7,
    GPS Speed (east)    11
    GPS Speed (north)   -6
    GPS Speed (up)      9
    Checksum             *17
*/
struct PLSR2457 {
    bool isValid;
    int16_t fieldValidity;

    int16_t gps_speed_east;
    int16_t gps_speed_north;
    int16_t gps_speed_up;
};

/*
    $GPGGA
    Global positioning system fixed data
    Message ID          $GPGGA
    UTC Time            171102.081 (hhmmss.sss)
    Latitude            4835.7692 (ddmm.mmmm)
    N/S indicator       N (N=north, S=south)
    Longitude           00220.9694 (dddmm.mmmm)
    E/W indocator       E (E=east, W=west)
    Position fix        1  (0 = invalid
                            1 = GPS fix (SPS)
                            2 = DGPS fix
                            3 = PPS fix
                            4 = Real Time Kinematic
                            5 = Float RTK
                            6 = estimated (dead reckoning)
                            7 = Manual input mode
                            8 = Simulation mode)
    Satellites used     04 (0 to 12)
    HDOP                3.7 (horizontal dilution of precision)
    MSL Altitude        101.6 (Above Mean Sea Level)
    Unit                M (meters)
    Geoid separation    47.4 (Height of geoid (mean sea level) above WGS84 ellipsoid
)    Unit                M (meters)
    Age of diff corr.   (tims since the last DGPS update)
    Station ID          0000 (DGPS Station ID number)
    Checksum            *52
*/
struct GPGGA {
    bool isValid;
    int16_t fieldValidity;

    char utc_time[15];
    char latitude[15];
    char north_south_indicator;
    char longitude[15];
    char east_west_indicator;
    int16_t position_fix;
    int16_t satellites_used;
    float hdop;
    float msl_altitude;
    char msl_unit[5];
    float geoid_separation;
    char geoid_unit[5];
    float age_of_diff_corr;
    int16_t station_id;
};

/*
    $GPGSA
    GNSS DOP and active satellites
    Sentence ID         $GPGSA
    Mode 1              A (A=automatic, allowed to switch 2D/3D, M=manual, forced to operate in 2D/3D)
    Mode 2              3 (1=fix not available, 2=2D, 3=3D)
    Id of satellite     19 (Satellite used on channel 1)
    Id of satellite     03 (Satellite used on channel 2)
    Id of satellite     14 (Satellite used on channel 3)
    Id of satellite     32 (Satellite used on channel 4)
    Id of satellite      (Satellite used on channel 5)
    Id of satellite      (Satellite used on channel 6)
    Id of satellite      (Satellite used on channel 7)
    Id of satellite      (Satellite used on channel 8)
    Id of satellite      (Satellite used on channel 9)
    Id of satellite      (Satellite used on channel 10)
    Id of satellite      (Satellite used on channel 11)
    Id of satellite      (Satellite used on channel 12)
    PDOP                5.7 (Position Dilution of Precision, 1.0 is perfect)
    HDOP                3.7 (Horizontal Dilution of Precision, 1.0 is perfect)
    VDOP                4.3 (Vertical Dilution of Precision, 1.0 is perfect)
    Checksum            *3C
*/
struct GPGSA {
    bool isValid;
    int16_t fieldValidity;

    char mode1;
    int16_t mode2;
    int16_t sat_channel_1;
    int16_t sat_channel_2;
    int16_t sat_channel_3;
    int16_t sat_channel_4;
    int16_t sat_channel_5;
    int16_t sat_channel_6;
    int16_t sat_channel_7;
    int16_t sat_channel_8;
    int16_t sat_channel_9;
    int16_t sat_channel_10;
    int16_t sat_channel_11;
    int16_t sat_channel_12;
    float pdop;
    float hdop;
    float vdop;
};

/*
    $GPGSV
    Satellites in View. SNR (Signal to Noise Ratio) field is often used as "Signal Strength", higher is better.
    Multiple sentences can be emitted, 4 satellites per sentence.
    Sentence ID         $GPGSV
    Number of GSV msgs  3 (from 1 to 3)
    Message number      1 (from 1 to 3)
    Satellites in view  12
    Satellite ID        11 (channel 1, from 01 to 32)
    Elevation           77 ((degrees) channel 1, from 01 to 90)
    Azimuth             288 ((degrees) channel 1, from 000 to 359)
    SNR                 ((db-Hz) channel 1, from 00 to 99, null when not tracking)
    Satellite ID        32 (channel 2, from 01 to 32)
    Elevation           58 ((degrees) channel 2, from 01 to 90)
    Azimuth             207 ((degrees) channel 2, from 000 to 359)
    SNR                 34 ((db-Hz) channel 2, from 00 to 99, null when not tracking)
    Satellite ID        19 (channel 3, from 01 to 32)
    Elevation           49 ((degrees) channel 3, from 01 to 90)
    Azimuth             154 ((degrees) channel 3, from 000 to 359)
    SNR                 40 ((db-Hz) channel 3, from 00 to 99, null when not tracking)
    Satellite ID        14 (channel 4, from 01 to 32)
    Elevation           35 ((degrees) channel 4, from 01 to 90)
    Azimuth             074 ((degrees) channel 4, from 000 to 359)
    SNR                 39 ((db-Hz) channel 4, from 00 to 99, null when not tracking)
    Checksum            *7F
*/
struct GPGSV {
    bool isValid;
    int16_t fieldValidity;

    int16_t number_of_messages;
    int16_t message_idx;
    int16_t sats_in_view;
    int16_t sat1_id;
    int16_t sat1_elevation;
    int16_t sat1_azimuth;
    int16_t sat1_snr;
    int16_t sat2_id;
    int16_t sat2_elevation;
    int16_t sat2_azimuth;
    int16_t sat2_snr;
    int16_t sat3_id;
    int16_t sat3_elevation;
    int16_t sat3_azimuth;
    int16_t sat3_snr;
    int16_t sat4_id;
    int16_t sat4_elevation;
    int16_t sat4_azimuth;
    int16_t sat4_snr;
};

/*
    $HCHDG
    Deviation and variation.
    Rate: default 1Hz, maxmimum 1Hz.
    Sentence ID         $HCHDG
    Heading             210 ((degree) magnetic sensor heading)
    Deviation           ((degree) magnetic deviation)
    Deviation direction (E=easterly, W=westerly)
    Variation           0.2 ((degree) magnetic variation)
    Variation direction W (E=easterly, W=westerly)
    Checksum            *24
*/
struct HCHDG {
    bool isValid;
    int16_t fieldValidity;

    int16_t heading;
    float deviation;
    char dev_direction;
    float variation;
    char var_direction;
};

/*
    $GPRMC
    Recommended Minimum Specific GNSS Data
    Sentence ID         $GPRMC
    UTC Time            171102.081 (hhmmss.sss)
    Status              A (A=datavalid, V=data not valid)
    Latitude            4835.7692 (ddmm.mmmm)
    N/S indicator       N (N=north, S=south)
    Longitude           00220.9694 (dddmm.mmmm)
    E/W Indicator       E (E=east, W=West)
    Speed over ground   0.24 (knots)
    Course over ground  210 ((degrees) Not valid at low speed. LS20126 may derive it from magnetic heading in this case)
    Date                121011 (ddmmyy)
    Magnetic variation  0.2 (degree)
    Variation sense     W (W=west, E=east)
    Mode                A (A=autonomous, D=DGPS, E=DR)
    Checksum            *33
*/
struct GPRMC {
    bool isValid;
    int16_t fieldValidity;

    char utc_time[15];
    char status;
    char latitude[15];
    char north_south_indicator;
    char longitude[15];
    char east_west_indicator;
    float speed_over_ground;
    float course_over_ground;
    char date[10];
    float magnetic_variation;
    char variation_sense;
    char mode;
};

#define GET_BIT(x, pos) (((x)&(1<<(pos)))!=0)
#define SET_BIT(x, pos) ((x)|(1<<(pos)))
#define IS_DIGIT(c) ((c)>='0' && (c)<='9')

class NMEAParser {
public:
    NMEAParser();

    bool check_checksum(const char *str);
    char generate_checksum(const char *str);

    //central dispatcher
    bool dispatch(const String str);
    bool dispatch(const char *str);

    //types
    enum STRINGS_TYPES {UNKNOWN=0, TYPE_PLSR2451, TYPE_PLSR2452, TYPE_PLSR2457, TYPE_GPGGA, TYPE_GPGSA, TYPE_GPGSV, TYPE_HCHDG, TYPE_GPRMC};
    STRINGS_TYPES getLastProcessedType() {return last_processed;}


    //parsers
    bool parse_plsr2451(const char *str);
    bool parse_plsr2452(const char *str);
    bool parse_plsr2457(const char *str);
    bool parse_gpgga(const char *str);
    bool parse_gpgsa(const char *str);
    bool parse_gpgsv(const char *str);
    bool parse_hchdg(const char *str);
    bool parse_gprmc(const char *str);

    PLSR2451 last_plsr2451 = {};
    PLSR2452 last_plsr2452 = {};
    PLSR2457 last_plsr2457 = {};
    GPGGA    last_gpgga = {};
    GPGSA    last_gpgsa = {};
    GPGSV    last_gpgsv = {};
    HCHDG    last_hchdg = {};
    GPRMC    last_gprmc = {};

private:
    STRINGS_TYPES last_processed = UNKNOWN;

    int16_t checksum = 0;
    int16_t scanned = 0;

    //string parsing subfunctions
    int16_t my_atoh(char a);
    char *my_strncpy(char *dst, const char *src, int16_t n);
    int16_t my_strlen(const char* str);
    int32_t my_atoi(const char *str);
    float my_atof(const char *s);

    //main parser
    int16_t my_sscanf(int16_t *field_validity, const char *src, const char *format, ... );

    //calculus tools
};

/*

Waiting for lock...

$PLSR,245,1,215,0,1042,12,-16,260,20,0,2*2F
$PLSR,245,2,-3345,2354,188,-2362,-3335,-251,7,-313,4084*16
$PLSR,245,7,,,*35
$GPGGA,000034.106,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000034.106,V,,,,,,,150209,,,N*42
$PLSR,245,1,215,0,1040,12,-15,261,19,0,2*25
$PLSR,245,2,-3342,2360,187,-2368,-3333,-234,16,-299,4085*24
$PLSR,245,7,,,*35
$GPGGA,000035.116,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,215,,,,*5A
$GPRMC,000035.116,V,,,,,,,150209,,,N*42
$PLSR,245,1,215,0,1041,12,-15,261,19,0,2*24
$PLSR,245,2,-3349,2351,187,-2359,-3340,-234,17,-299,4085*2A
$PLSR,245,7,,,*35
$GPGGA,000036.108,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000036.108,V,,,,,,,150209,,,N*4E
$PLSR,245,1,215,0,1040,13,-15,261,19,0,2*24
$PLSR,245,2,-3347,2352,203,-2361,-3338,-234,30,-308,4084*21
$PLSR,245,7,,,*35
$GPGGA,000037.107,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000037.107,V,,,,,,,150209,,,N*40
$PLSR,245,1,215,0,1041,12,-16,260,20,0,2*2C
$PLSR,245,2,-3344,2357,188,-2365,-3334,-251,7,-313,4084*12
$PLSR,245,7,,,*35
$GPGGA,000038.117,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000038.117,V,,,,,,,150209,,,N*4E
$PLSR,245,1,215,0,1041,11,-16,260,20,0,2*2F
$PLSR,245,2,-3340,2365,172,-2372,-3330,-251,-5,-304,4085*38
$PLSR,245,7,,,*35
$GPGGA,000039.107,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000039.107,V,,,,,,,150209,,,N*4E
$PLSR,245,1,213,0,1052,-2,14,229,20,0,2*16
$PLSR,245,2,-3403,2278,-35,-2276,-3396,250,108,227,4089*0F
$PLSR,245,7,,,*35
$GPGGA,000040.105,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,213,,,,*5C
$GPRMC,000040.105,V,,,,,,,150209,,,N*42
$PLSR,245,1,215,0,1049,-81,5,295,20,0,2*16
$PLSR,245,2,-2967,2607,-1084,-2717,-3063,66,-768,766,3949*15
$PLSR,245,7,,,*35
$GPGGA,000041.115,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000041.115,V,,,,,,,150209,,,N*42
$PLSR,245,1,208,0,1054,49,-13,235,19,0,2*25
$PLSR,245,2,-3545,1873,834,-1951,-3593,-221,630,-588,4004*19
$PLSR,245,7,,,*35
$GPGGA,000042.108,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,208,,,,*56
$GPRMC,000042.108,V,,,,,,,150209,,,N*4D
$PLSR,245,1,215,0,1027,25,-75,302,20,0,2*28
$PLSR,245,2,-3129,2622,328,-2614,-2996,-984,-388,-961,3963*36
$PLSR,245,7,,,*35
$GPGGA,000043.107,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,215,,,,*5A
$GPRMC,000043.107,V,,,,,,,150209,,,N*43
$PLSR,245,1,221,0,1048,13,-68,240,20,0,2*28
$PLSR,245,2,-3060,2714,213,-2659,-2909,-1115,-586,-971,3936*00
$PLSR,245,7,,,*35
$GPGGA,000044.118,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,221,,,,*5D
$GPRMC,000044.118,V,,,,,,,150209,,,N*4A
$PLSR,245,1,224,0,1044,8,-69,266,20,0,2*1E
$PLSR,245,2,-3045,2738,119,-2674,-2928,-1028,-601,-841,3963*0D
$PLSR,245,7,,,*35
$GPGGA,000045.106,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,224,,,,*58
$GPRMC,000045.106,V,,,,,,,150209,,,N*44
$PLSR,245,1,226,0,1048,11,-78,245,20,0,2*29
$PLSR,245,2,-2968,2817,175,-2726,-2794,-1241,-734,-1015,3900*3A
$PLSR,245,7,,,*35
$GPGGA,000046.105,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,226,,,,*5A
$GPRMC,000046.105,V,,,,,,,150209,,,N*44
$PLSR,245,1,220,0,1029,26,-83,250,20,0,2*2C
$PLSR,245,2,-3016,2741,402,-2709,-2790,-1284,-584,-1211,3868*32
$PLSR,245,7,,,*35
$GPGGA,000047.116,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,220,,,,*5C
$GPRMC,000047.116,V,,,,,,,150209,,,N*47
$PLSR,245,1,217,0,1009,50,-76,233,20,0,2*24
$PLSR,245,2,-3216,2399,818,-2532,-2968,-1244,-135,-1482,3815*31
$PLSR,245,7,,,*35
$GPGGA,000048.106,,,,,0,00,,,M,0.0,M,,0000*5D
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,217,,,,*58
$GPRMC,000048.106,V,,,,,,,150209,,,N*49
$PLSR,245,1,188,0,1027,94,-21,382,20,0,2*2C
$PLSR,245,2,-3617,1654,977,-1750,-3695,-218,793,-609,3971*1E
$PLSR,245,7,,,*35
$GPGGA,000049.108,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,188,,,,*5D
$GPRMC,000049.108,V,,,,,,,150209,,,N*46
$PLSR,245,1,177,0,1052,163,50,230,20,0,2*34
$PLSR,245,2,-3356,283,2332,156,-4030,715,2344,674,3291*31
$PLSR,245,7,,,*35
$GPGGA,000050.116,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,177,,,,*5D
$GPRMC,000050.116,V,,,,,,,150209,,,N*41
$PLSR,245,1,163,0,1082,170,140,153,20,0,2*08
$PLSR,245,2,-3023,950,2596,764,-3408,2138,2657,2062,2337*31
$PLSR,245,7,,,*35
$GPGGA,000051.106,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,163,,,,*58
$GPRMC,000051.106,V,,,,,,,150209,,,N*41
$PLSR,245,1,184,0,1034,66,-55,338,20,0,2*2D
$PLSR,245,2,-3553,1886,775,-2006,-3512,-646,365,-939,3970*14
$PLSR,245,7,,,*35
$GPGGA,000052.106,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,184,,,,*51
$GPRMC,000052.106,V,,,,,,,150209,,,N*42
$PLSR,245,1,202,0,1056,60,-28,262,20,0,2*26
$PLSR,245,2,-3686,1537,909,-1657,-3721,-424,665,-749,3971*15
$PLSR,245,7,,,*35
$GPGGA,000053.117,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,202,,,,*5C
$GPRMC,000053.117,V,,,,,,,150209,,,N*43
$PLSR,245,1,201,0,1054,60,-6,252,20,0,2*18
$PLSR,245,2,-3729,1404,948,-1464,-3824,-94,852,-424,3984*2A
$PLSR,245,7,,,*35
$GPGGA,000054.106,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,201,,,,*5F
$GPRMC,000054.106,V,,,,,,,150209,,,N*44
$PLSR,245,1,198,0,1054,75,-18,254,20,0,2*26
$PLSR,245,2,-3758,1147,1157,-1271,-3883,-277,1018,-613,3919*1C
$PLSR,245,7,,,*35
$GPGGA,000055.109,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,198,,,,*5C
$GPRMC,000055.109,V,,,,,,,150209,,,N*4A
$PLSR,245,1,199,0,1056,73,-15,253,20,0,2*29
$PLSR,245,2,-3758,1176,1133,-1286,-3883,-233,1006,-569,3929*16
$PLSR,245,7,,,*35
$GPGGA,000056.117,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,199,,,,*5D
$GPRMC,000056.117,V,,,,,,,150209,,,N*46
$PLSR,245,1,200,0,1052,69,-20,256,20,0,2*26
$PLSR,245,2,-3733,1311,1062,-1432,-3825,-308,893,-651,3943*20
$PLSR,245,7,,,*35
$GPGGA,000057.106,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,200,,,,*5E
$GPRMC,000057.106,V,,,,,,,150209,,,N*47
$PLSR,245,1,199,0,1056,67,-16,262,20,0,2*2D
$PLSR,245,2,-3736,1338,1013,-1437,-3827,-241,866,-575,3961*2C
$PLSR,245,7,,,*35
$GPGGA,000058.106,,,,,0,00,,,M,0.0,M,,0000*5C
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,199,,,,*5D
$GPRMC,000058.106,V,,,,,,,150209,,,N*48
$PLSR,245,1,199,0,1054,70,-14,254,20,0,2*2E
$PLSR,245,2,-3746,1249,1086,-1351,-3859,-217,957,-556,3943*2F
$PLSR,245,7,,,*35
$GPGGA,000059.116,,,,,0,00,,,M,0.0,M,,0000*5C
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,199,,,,*5D
$GPRMC,000059.116,V,,,,,,,150209,,,N*48
$PLSR,245,1,197,0,1055,75,-12,254,20,0,2*22
$PLSR,245,2,-3761,1137,1158,-1237,-3900,-185,1050,-519,3925*12
$PLSR,245,7,,,*35
$GPGGA,000100.106,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,197,,,,*53
$GPRMC,000100.106,V,,,,,,,150209,,,N*44
$PLSR,245,1,189,0,1080,102,36,220,20,0,2*3C
$PLSR,245,2,-3704,387,1704,-147,-4048,601,1740,482,3676*16
$PLSR,245,7,,,*35
$GPGGA,000101.107,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,189,,,,*5C
$GPRMC,000101.107,V,,,,,,,150209,,,N*44
$PLSR,245,1,175,0,1033,131,-34,326,21,0,2*1E
$PLSR,245,2,-3747,656,1520,-858,-3985,-394,1415,-678,3783*12
$PLSR,245,7,,,*35
$GPGGA,000102.118,,,,,0,00,,,M,0.0,M,,0000*5D
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,175,,,,*5F
$GPRMC,000102.118,V,,,,,,,150209,,,N*49
$PLSR,245,1,156,0,1062,194,84,95,20,0,2*08
$PLSR,245,2,-2213,352,3428,1760,-3387,1484,2964,2274,1679*0E
$PLSR,245,7,,,*35
$GPGGA,000103.105,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,156,,,,*5E
$GPRMC,000103.105,V,,,,,,,150209,,,N*44
$PLSR,245,1,255,6,666,73,128,256,20,0,2*0F
$PLSR,245,2,-676,3910,1012,-3534,-1069,1774,1959,-580,3549*36
$PLSR,245,7,,,*35
$GPGGA,000104.106,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,255,,,,*5E
$GPRMC,000104.106,V,,,,,,,150209,,,N*40
$PLSR,245,1,235,6,666,92,125,214,20,0,2*0D
$PLSR,245,2,-990,3710,1425,-3232,-1606,1937,2314,-656,3316*3C
$PLSR,245,7,,,*35
$GPGGA,000105.114,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,235,,,,*58
$GPRMC,000105.114,V,,,,,,,150209,,,N*42
$PLSR,245,1,250,6,667,38,150,191,21,0,2*02
$PLSR,245,2,-1075,3901,633,-3002,-1232,2499,2572,191,3182*15
$PLSR,245,7,,,*35
$GPGGA,000106.106,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,250,,,,*5B
$GPRMC,000106.106,V,,,,,,,150209,,,N*42
$PLSR,245,1,230,6,671,56,218,117,20,0,2*0B
$PLSR,245,2,-1714,3607,904,-1407,-1550,3520,3443,1162,1889*2A
$PLSR,245,7,,,*35
$GPGGA,000107.106,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,230,,,,*5D
$GPRMC,000107.106,V,,,,,,,150209,,,N*43
$PLSR,245,1,243,6,671,24,196,177,20,0,2*09
$PLSR,245,2,-1346,3851,370,-2501,-1165,3028,2952,769,2734*1C
$PLSR,245,7,,,*35
$GPGGA,000108.119,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,243,,,,*59
$GPRMC,000108.119,V,,,,,,,150209,,,N*42
$PLSR,245,1,241,6,670,38,193,170,21,0,2*04
$PLSR,245,2,-1403,3800,598,-2384,-1358,3041,3020,693,2678*16
$PLSR,245,7,,,*35
$GPGGA,000109.108,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000109.108,V,,,,,,,150209,,,N*43
$PLSR,245,1,240,6,669,40,184,180,21,0,2*0B
$PLSR,245,2,-1344,3817,629,-2552,-1375,2893,2908,557,2830*12
$PLSR,245,7,,,*35
$GPGGA,000110.106,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,240,,,,*5A
$GPRMC,000110.106,V,,,,,,,150209,,,N*45
$PLSR,245,1,241,6,669,42,184,180,21,0,2*08
$PLSR,245,2,-1372,3802,659,-2534,-1415,2890,2910,560,2827*1D
$PLSR,245,7,,,*35
$GPGGA,000111.116,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000111.116,V,,,,,,,150209,,,N*45
$PLSR,245,1,242,6,669,43,180,182,21,0,2*0C
$PLSR,245,2,-1300,3824,678,-2604,-1388,2840,2882,470,2872*1B
$PLSR,245,7,,,*35
$GPGGA,000112.106,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,242,,,,*58
$GPRMC,000112.106,V,,,,,,,150209,,,N*47
$PLSR,245,1,241,6,669,44,181,183,20,0,2*09
$PLSR,245,2,-1335,3810,690,-2588,-1420,2839,2880,489,2871*17
$PLSR,245,7,,,*35
$GPGGA,000113.104,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000113.104,V,,,,,,,150209,,,N*44
$PLSR,245,1,241,6,669,46,181,184,21,0,2*0D
$PLSR,245,2,-1331,3806,718,-2590,-1439,2828,2880,464,2875*13
$PLSR,245,7,,,*35
$GPGGA,000114.117,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000114.117,V,,,,,,,150209,,,N*41
$PLSR,245,1,241,6,669,46,180,183,20,0,2*0A
$PLSR,245,2,-1335,3802,722,-2587,-1444,2827,2880,465,2874*19
$PLSR,245,7,,,*35
$GPGGA,000115.107,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,00*79
$HCHDG,241,,,,*5B
$GPRMC,000115.107,V,,,,,,,150209,,,N*41
$PLSR,245,1,239,6,669,46,180,182,21,0,2*05
$PLSR,245,2,-1353,3796,724,-2571,-1456,2835,2886,482,2866*18
$PLSR,245,7,,,*35
$GPGGA,000116.108,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,239,,,,*54
$GPRMC,000116.108,V,,,,,,,150209,,,N*4D
$PLSR,245,1,241,6,669,48,178,185,21,0,2*04
$PLSR,245,2,-1321,3802,752,-2616,-1460,2792,2861,420,2901*1A
$PLSR,245,7,,,*35
$GPGGA,000117.063,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000117.063,V,,,,,,,150209,,,N*40
$PLSR,245,1,240,6,669,49,179,184,20,0,2*05
$PLSR,245,2,-1299,3807,768,-2606,-1454,2805,2881,400,2884*13
$PLSR,245,7,,,*35
$GPGGA,000118.063,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,240,,,,*5A
$GPRMC,000118.063,V,,,,,,,150209,,,N*4F
$PLSR,245,1,241,6,669,49,178,187,20,0,2*06
$PLSR,245,2,-1312,3804,763,-2634,-1464,2774,2850,397,2915*1E
$PLSR,245,7,,,*35
$GPGGA,000119.062,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,241,,,,*5B
$GPRMC,000119.062,V,,,,,,,150209,,,N*4F
$PLSR,245,1,240,6,669,49,179,185,21,0,2*05
$PLSR,245,2,-1311,3803,766,-2609,-1462,2798,2872,407,2892*15
$PLSR,245,7,,,*35
$GPGGA,000120.063,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,34,14,,,38*72
$HCHDG,240,,,,*5A
$GPRMC,000120.063,V,,,,,,,150209,,,N*44
$PLSR,245,1,240,6,669,48,179,185,21,0,2*04
$PLSR,245,2,-1326,3801,750,-2607,-1461,2800,2867,429,2894*1B
$PLSR,245,7,,,*35
$GPGGA,000121.062,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,240,,,,*5A
$GPRMC,000121.062,V,,,,,,,150209,,,N*44
$PLSR,245,1,239,6,670,49,180,184,21,0,2*04
$PLSR,245,2,-1341,3793,766,-2582,-1480,2814,2883,438,2876*19
$PLSR,245,7,,,*35
$GPGGA,000122.062,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,239,,,,*54
$GPRMC,000122.062,V,,,,,,,150209,,,N*47
$PLSR,245,1,239,6,670,48,181,183,21,0,2*03
$PLSR,245,2,-1364,3787,750,-2561,-1482,2831,2891,473,2862*1F
$PLSR,245,7,,,*35
$GPGGA,000123.063,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,239,,,,*54
$GPRMC,000123.063,V,,,,,,,150209,,,N*47
$PLSR,245,1,240,6,670,49,182,181,21,0,2*0D
$PLSR,245,2,-1361,3785,768,-2534,-1489,2852,2916,472,2837*12
$PLSR,245,7,,,*35
$GPGGA,000124.062,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,240,,,,*5A
$GPRMC,000124.062,V,,,,,,,150209,,,N*41
$PLSR,245,1,239,6,669,49,182,182,21,0,2*08
$PLSR,245,2,-1330,3796,766,-2553,-1468,2845,2913,446,2845*15
$PLSR,245,7,,,*35
$GPGGA,000125.062,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,35,14,,,38*73
$HCHDG,239,,,,*54
$GPRMC,000125.062,V,,,,,,,150209,,,N*40
$PLSR,245,1,240,6,670,49,182,182,21,0,2*0E
$PLSR,245,2,-1364,3785,766,-2541,-1491,2845,2909,472,2845*1F
$PLSR,245,7,,,*35
$GPGGA,000126.063,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,240,,,,*5A
$GPRMC,000126.063,V,,,,,,,150209,,,N*42
$PLSR,245,1,239,6,670,48,184,179,21,0,2*03
$PLSR,245,2,-1364,3787,752,-2504,-1474,2886,2940,501,2807*11
$PLSR,245,7,,,*35
$GPGGA,000127.062,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,239,,,,*54
$GPRMC,000127.062,V,,,,,,,150209,,,N*42
$PLSR,245,1,239,6,670,48,184,178,21,0,2*02
$PLSR,245,2,-1396,3775,754,-2483,-1495,2893,2943,528,2799*12
$PLSR,245,7,,,*35
$GPGGA,000128.062,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,239,,,,*54
$GPRMC,000128.062,V,,,,,,,150209,,,N*4D
$PLSR,245,1,243,6,671,36,200,179,21,0,2*09
$PLSR,245,2,-1373,3820,544,-2434,-1305,3025,2995,690,2707*1F
$PLSR,245,7,,,*35
$GPGGA,163834.811,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,243,,,,*59
$GPRMC,163834.811,V,,,,,,,180209,,,N*4C
$PLSR,245,1,237,6,671,52,190,173,21,0,2*08
$PLSR,245,2,-1442,3745,812,-2358,-1550,2968,3022,577,2703*18
$PLSR,245,7,,,*35
$GPGGA,163835.810,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,35,14,,,38*73
$HCHDG,237,,,,*5A
$GPRMC,163835.810,V,,,,,,,180209,,,N*4C
$PLSR,245,1,235,6,670,54,188,177,21,0,2*00
$PLSR,245,2,-1426,3745,838,-2406,-1568,2919,2991,523,2748*1D
$PLSR,245,7,,,*35
$GPGGA,163836.810,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163836.810,V,,,,,,,180209,,,N*4F
$PLSR,245,1,237,6,670,57,180,180,21,0,2*01
$PLSR,245,2,-1389,3746,895,-2495,-1599,2826,2935,413,2826*19
$PLSR,245,7,,,*35
$GPGGA,163837.811,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,237,,,,*5A
$GPRMC,163837.811,V,,,,,,,180209,,,N*4F
$PLSR,245,1,237,6,670,57,180,181,21,0,2*00
$PLSR,245,2,-1370,3755,892,-2512,-1586,2819,2930,395,2835*18
$PLSR,245,7,,,*35
$GPGGA,163838.807,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,237,,,,*5A
$GPRMC,163838.807,V,,,,,,,180209,,,N*47
$PLSR,245,1,238,6,669,58,180,181,21,0,2*08
$PLSR,245,2,-1406,3738,907,-2494,-1620,2816,2930,414,2832*1E
$PLSR,245,7,,,*35
$GPGGA,163839.810,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,238,,,,*55
$GPRMC,163839.810,V,,,,,,,180209,,,N*40
$PLSR,245,1,237,6,669,58,179,183,21,0,2*03
$PLSR,245,2,-1376,3749,905,-2531,-1602,2793,2911,379,2856*1B
$PLSR,245,7,,,*35
$GPGGA,163840.811,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,35,14,,,38*73
$HCHDG,237,,,,*5A
$GPRMC,163840.811,V,,,,,,,180209,,,N*4F
$PLSR,245,1,237,6,669,58,179,181,21,0,2*01
$PLSR,245,2,-1355,3756,910,-2522,-1590,2808,2930,368,2839*1D
$PLSR,245,7,,,*35
$GPGGA,163841.810,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,237,,,,*5A
$GPRMC,163841.810,V,,,,,,,180209,,,N*4F
$PLSR,245,1,237,6,669,58,180,180,21,0,2*06
$PLSR,245,2,-1397,3740,910,-2488,-1615,2824,2938,410,2824*19
$PLSR,245,7,,,*35
$GPGGA,163842.810,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,237,,,,*5A
$GPRMC,163842.810,V,,,,,,,180209,,,N*4C
$PLSR,245,1,238,6,669,58,179,183,21,0,2*0C
$PLSR,245,2,-1349,3760,905,-2543,-1584,2793,2914,357,2856*1D
$PLSR,245,7,,,*35
$GPGGA,163843.811,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,238,,,,*55
$GPRMC,163843.811,V,,,,,,,180209,,,N*4C
$PLSR,245,1,237,6,669,59,180,180,21,0,2*07
$PLSR,245,2,-1355,3752,925,-2501,-1598,2822,2946,368,2822*15
$PLSR,245,7,,,*35
$GPGGA,163844.811,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,237,,,,*5A
$GPRMC,163844.811,V,,,,,,,180209,,,N*4B
$PLSR,245,1,236,6,670,60,181,180,21,0,2*05
$PLSR,245,2,-1412,3729,937,-2466,-1644,2827,2950,410,2812*15
$PLSR,245,7,,,*35
$GPGGA,163845.811,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,35,14,,,38*73
$HCHDG,236,,,,*5B
$GPRMC,163845.811,V,,,,,,,180209,,,N*4A
$PLSR,245,1,236,6,670,59,182,179,21,0,2*0A
$PLSR,245,2,-1432,3724,922,-2444,-1644,2845,2958,444,2798*1E
$PLSR,245,7,,,*35
$GPGGA,163846.811,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163846.811,V,,,,,,,180209,,,N*49
$PLSR,245,1,236,6,670,59,183,177,21,0,2*05
$PLSR,245,2,-1422,3728,924,-2424,-1635,2868,2980,448,2774*11
$PLSR,245,7,,,*35
$GPGGA,163847.810,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163847.810,V,,,,,,,180209,,,N*49
$PLSR,245,1,236,6,670,59,183,179,21,0,2*0B
$PLSR,245,2,-1432,3725,919,-2438,-1640,2853,2964,450,2791*1C
$PLSR,245,7,,,*35
$GPGGA,163848.810,,,,,0,00,,,M,0.0,M,,0000*5F
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163848.810,V,,,,,,,180209,,,N*46
$PLSR,245,1,236,6,670,59,183,179,21,0,2*0B
$PLSR,245,2,-1432,3725,919,-2438,-1640,2853,2964,450,2791*1C
$PLSR,245,7,,,*35
$GPGGA,163849.811,,,,,0,00,,,M,0.0,M,,0000*5F
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163849.811,V,,,,,,,180209,,,N*46
$PLSR,245,1,235,6,670,59,182,178,21,0,2*08
$PLSR,245,2,-1409,3732,924,-2445,-1629,2852,2968,429,2790*1B
$PLSR,245,7,,,*35
$GPGGA,163850.810,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,1,1,02,11,,,35,14,,,38*73
$HCHDG,235,,,,*58
$GPRMC,163850.810,V,,,,,,,180209,,,N*4F
$PLSR,245,1,235,6,670,60,185,176,22,0,2*08
$PLSR,245,2,-1444,3716,937,-2385,-1655,2889,3001,472,2748*1C
$PLSR,245,7,,,*35
$GPGGA,163851.810,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163851.810,V,,,,,,,091111,,,N*45
$PLSR,245,1,235,6,670,60,186,176,21,0,2*08
$PLSR,245,2,-1439,3719,934,-2381,-1648,2896,3007,474,2741*15
$PLSR,245,7,,,*35
$GPGGA,163852.811,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163852.811,V,,,,,,,091111,,,N*47
$PLSR,245,1,236,6,670,59,183,178,21,0,2*0A
$PLSR,245,2,-1424,3726,922,-2431,-1636,2860,2972,447,2782*1B
$PLSR,245,7,,,*35
$GPGGA,163853.810,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163853.810,V,,,,,,,091111,,,N*47
$PLSR,245,1,236,6,670,61,181,178,21,0,2*03
$PLSR,245,2,-1425,3717,957,-2437,-1664,2839,2967,418,2792*18
$PLSR,245,7,,,*35
$GPGGA,163854.809,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163854.809,V,,,,,,,091111,,,N*48
$PLSR,245,1,234,6,670,73,183,183,21,0,2*04
$PLSR,245,2,-1438,3669,1112,-2409,-1788,2787,2984,324,2787*21
$PLSR,245,7,,,*35
$GPGGA,163855.811,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,38*78
$GPGSV,3,2,12,11,30,259,35,05,25,051,,22,24,133,,30,23,092,*78
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,234,,,,*59
$GPRMC,163855.811,V,,,,,,,091111,,,N*40
$PLSR,245,1,232,6,670,68,185,172,21,0,2*00
$PLSR,245,2,-1462,3675,1064,-2303,-1755,2897,3056,435,2693*24
$PLSR,245,7,,,*35
$GPGGA,163856.810,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,232,,,,*5F
$GPRMC,163856.810,V,,,,,,,091111,,,N*42
$PLSR,245,1,233,6,670,67,181,176,21,0,2*0E
$PLSR,245,2,-1449,3682,1050,-2381,-1745,2838,3000,393,2760*29
$PLSR,245,7,,,*35
$GPGGA,163857.810,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,233,,,,*5E
$GPRMC,163857.810,V,,,,,,,091111,,,N*43
$PLSR,245,1,233,6,670,66,183,176,21,0,2*0D
$PLSR,245,2,-1484,3675,1030,-2355,-1751,2857,3004,442,2748*26
$PLSR,245,7,,,*35
$GPGGA,163858.811,,,,,0,00,,,M,0.0,M,,0000*5F
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,233,,,,*5E
$GPRMC,163858.811,V,,,,,,,091111,,,N*4D
$PLSR,245,1,233,6,670,66,183,174,21,0,2*0F
$PLSR,245,2,-1472,3677,1035,-2341,-1744,2871,3021,440,2730*27
$PLSR,245,7,,,*35
$GPGGA,163859.811,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,233,,,,*5E
$GPRMC,163859.811,V,,,,,,,091111,,,N*4C
$PLSR,245,1,233,6,670,65,185,175,21,0,2*0B
$PLSR,245,2,-1467,3686,1013,-2343,-1724,2883,3022,453,2727*27
$PLSR,245,7,,,*35
$GPGGA,163900.810,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,38*78
$GPGSV,3,2,12,11,30,259,36,05,25,051,,22,24,133,,30,23,092,*7B
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,233,,,,*5E
$GPRMC,163900.810,V,,,,,,,091111,,,N*40
$PLSR,245,1,232,6,669,69,182,174,22,0,2*0B
$PLSR,245,2,-1486,3659,1082,-2328,-1789,2855,3024,420,2730*27
$PLSR,245,7,,,*35
$GPGGA,163901.811,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,232,,,,*5F
$GPRMC,163901.811,V,,,,,,,091111,,,N*40
$PLSR,245,1,231,6,669,72,180,175,21,0,2*02
$PLSR,245,2,-1463,3654,1129,-2350,-1812,2823,3018,360,2745*2C
$PLSR,245,7,,,*35
$GPGGA,163902.810,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,231,,,,*5C
$GPRMC,163902.810,V,,,,,,,091111,,,N*42
$PLSR,245,1,233,6,669,71,173,174,21,0,2*0E
$PLSR,245,2,-1456,3654,1138,-2401,-1820,2774,2982,318,2790*29
$PLSR,245,7,,,*35
$GPGGA,163903.810,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,233,,,,*5E
$GPRMC,163903.810,V,,,,,,,091111,,,N*43
$PLSR,245,1,234,6,669,72,172,179,22,0,2*05
$PLSR,245,2,-1405,3673,1141,-2475,-1793,2726,2945,245,2836*26
$PLSR,245,7,,,*35
$GPGGA,163904.811,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,234,,,,*59
$GPRMC,163904.811,V,,,,,,,091111,,,N*45
$PLSR,245,1,236,6,669,71,170,187,21,0,2*04
$PLSR,245,2,-1341,3707,1107,-2597,-1730,2652,2870,166,2917*20
$PLSR,245,7,,,*35
$GPGGA,163905.811,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,38*78
$GPGSV,3,2,12,11,30,259,36,05,25,051,,22,24,133,,30,23,092,*7B
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,236,,,,*5B
$GPRMC,163905.811,V,,,,,,,091111,,,N*44
$PLSR,245,1,236,6,669,71,170,186,22,0,2*06
$PLSR,245,2,-1326,3712,1111,-2594,-1722,2660,2879,157,2910*2F
$PLSR,245,7,,,*35
$GPGGA,163906.810,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,236,,,,*5B
$GPRMC,163906.810,V,,,,,,,091111,,,N*46
$PLSR,245,1,235,6,669,71,172,184,22,0,2*05
$PLSR,245,2,-1393,3687,1111,-2530,-1767,2692,2904,229,2880*27
$PLSR,245,7,,,*35
$GPGGA,163907.811,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163907.811,V,,,,,,,091111,,,N*46
$PLSR,245,1,235,6,669,69,173,186,21,0,2*0C
$PLSR,245,2,-1381,3702,1073,-2554,-1732,2692,2888,238,2894*2F
$PLSR,245,7,,,*35
$GPGGA,163908.810,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163908.810,V,,,,,,,091111,,,N*48
$PLSR,245,1,235,6,670,69,174,185,21,0,2*00
$PLSR,245,2,-1419,3687,1074,-2520,-1757,2708,2899,277,2879*28
$PLSR,245,7,,,*35
$GPGGA,163909.810,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163909.810,V,,,,,,,091111,,,N*49
$PLSR,245,1,235,6,670,68,175,181,22,0,2*07
$PLSR,245,2,-1440,3682,1068,-2472,-1763,2748,2931,321,2843*21
$PLSR,245,7,,,*35
$GPGGA,163910.811,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,38*78
$GPGSV,3,2,12,11,30,259,37,05,25,051,,22,24,133,,30,23,092,*7A
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,235,,,,*58
$GPRMC,163910.811,V,,,,,,,091111,,,N*40
$PLSR,245,1,235,6,669,68,176,182,22,0,2*0F
$PLSR,245,2,-1431,3687,1062,-2477,-1753,2750,2931,318,2843*2D
$PLSR,245,7,,,*35
$GPGGA,163911.810,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163911.810,V,,,,,,,091111,,,N*40
$PLSR,245,1,234,6,670,68,176,182,21,0,2*05
$PLSR,245,2,-1433,3687,1062,-2477,-1754,2750,2931,319,2843*29
$PLSR,245,7,,,*35
$GPGGA,163912.811,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,234,,,,*59
$GPRMC,163912.811,V,,,,,,,091111,,,N*42
$PLSR,245,1,234,6,670,68,177,180,21,0,2*06
$PLSR,245,2,-1446,3680,1065,-2444,-1762,2773,2951,343,2820*23
$PLSR,245,7,,,*35
$GPGGA,163913.811,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,234,,,,*59
$GPRMC,163913.811,V,,,,,,,091111,,,N*43
$PLSR,245,1,235,6,669,68,178,180,22,0,2*03
$PLSR,245,2,-1396,3701,1062,-2462,-1726,2782,2961,309,2813*21
$PLSR,245,7,,,*35
$GPGGA,163914.810,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,235,,,,*58
$GPRMC,163914.810,V,,,,,,,091111,,,N*45
$PLSR,245,1,234,6,670,70,177,178,22,0,2*0B
$PLSR,245,2,-1457,3665,1100,-2412,-1793,2782,2971,341,2798*25
$PLSR,245,7,,,*35
$GPGGA,163915.810,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,38*78
$GPGSV,3,2,12,11,30,259,37,05,25,051,,22,24,133,,30,23,092,*7A
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,234,,,,*59
$GPRMC,163915.810,V,,,,,,,091111,,,N*44
$PLSR,245,1,233,6,670,71,179,177,22,0,2*0C
$PLSR,245,2,-1476,3655,1112,-2375,-1811,2803,2993,365,2772*2D
$PLSR,245,7,,,*35
$GPGGA,163916.811,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,233,,,,*5E
$GPRMC,163916.811,V,,,,,,,091111,,,N*46
$PLSR,245,1,204,6,673,117,218,77,21,0,2*0C
$PLSR,245,2,-1993,3063,1849,-192,-2205,3446,3573,1590,1217*2D
$PLSR,245,7,,,*35
$GPGGA,163917.811,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,204,,,,*5A
$GPRMC,163917.811,V,,,,,,,091111,,,N*47
$PLSR,245,1,200,6,667,205,88,143,22,0,2*03
$PLSR,245,2,-1217,2291,3169,-2300,-3103,1360,3163,-1375,2210*32
$PLSR,245,7,,,*35
$GPGGA,163918.811,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,200,,,,*5E
$GPRMC,163918.811,V,,,,,,,091111,,,N*48
$PLSR,245,1,256,6,667,210,35,163,21,0,2*03
$PLSR,245,2,317,2527,3208,-4059,-165,534,457,-3220,2490*11
$PLSR,245,7,,,*35
$GPGGA,163919.811,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,256,,,,*5D
$GPRMC,163919.811,V,,,,,,,091111,,,N*49
$PLSR,245,1,258,6,668,202,51,179,22,0,2*0B
$PLSR,245,2,141,2771,3012,-3978,-619,760,968,-2951,2669*13
$PLSR,245,7,,,*35
$GPGGA,163920.811,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,56,275,,31,40,174,,18,35,138,,14,30,079,29*78
$GPGSV,3,2,12,11,30,259,27,05,25,051,,22,24,133,,30,23,092,*7B
$GPGSV,3,3,12,12,21,092,,17,16,322,,20,15,297,,09,02,037,*76
$HCHDG,258,,,,*53
$GPRMC,163920.811,V,,,,,,,091111,,,N*43
$PLSR,245,1,260,6,669,209,70,172,21,0,2*01
$PLSR,245,2,-143,2716,3062,-3728,-1350,1025,1689,-2751,2520*0E
$PLSR,245,7,,,*35
$GPGGA,163921.811,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,260,,,,*58
$GPRMC,163921.811,V,,,,,,,091111,,,N*42
$PLSR,245,1,260,6,670,204,48,183,22,0,2*02
$PLSR,245,2,217,2777,3003,-4011,-448,706,806,-2978,2694*12
$PLSR,245,7,,,*35
$GPGGA,163922.711,,,,,0,00,,,M,0.0,M,,0000*5C
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,260,,,,*58
$GPRMC,163922.711,V,,,,,,,091111,,,N*4E
$PLSR,245,1,265,6,666,177,61,199,21,0,2*04
$PLSR,245,2,271,3108,2653,-3970,-432,914,972,-2631,2983*1F
$PLSR,245,7,,,*35
$GPGGA,163923.856,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,265,,,,*5D
$GPRMC,163923.856,V,,,,,,,091111,,,N*43
$PLSR,245,1,259,6,665,183,55,204,22,0,2*00
$PLSR,245,2,309,3081,2681,-4007,-298,806,800,-2683,2989*13
$PLSR,245,7,,,*35
$GPGGA,163924.815,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,259,,,,*52
$GPRMC,163924.815,V,,,,,,,091111,,,N*43
$PLSR,245,1,260,6,666,164,88,196,21,0,2*0B
$PLSR,245,2,-240,3247,2485,-3653,-1290,1333,1840,-2138,2970*0D
$PLSR,245,7,,,*35
$GPGGA,163925.815,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,90,179,30,11,57,198,32,20,41,265,,14,39,056,28*7F
$GPGSV,3,2,12,17,33,296,,31,28,106,,23,17,208,,18,12,084,*74
$GPGSV,3,3,12,05,04,017,,22,01,083,,03,,195,,30,-3,050,*61
$HCHDG,260,,,,*58
$GPRMC,163925.815,V,,,,,,,091111,,,N*42
$PLSR,245,1,264,6,668,-18,57,296,22,0,2*14
$PLSR,245,2,98,4087,-244,-4020,142,773,780,220,4015*05
$PLSR,245,7,,,*35
$GPGGA,163926.815,,,,,0,00,,,M,0.0,M,,0000*53
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,264,,,,*5C
$GPRMC,163926.815,V,,,,,,,091111,,,N*41
$PLSR,245,1,265,6,667,47,101,237,22,0,2*04
$PLSR,245,2,-73,4028,735,-3763,-356,1579,1617,-647,3706*06
$PLSR,245,7,,,*35
$GPGGA,163927.809,,,,,0,00,,,M,0.0,M,,0000*5F
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,265,,,,*5D
$GPRMC,163927.809,V,,,,,,,091111,,,N*4D
$PLSR,245,1,278,6,667,43,0,284,22,0,2*04
$PLSR,245,2,694,3990,613,-4036,701,0,-104,-604,4050*10
$PLSR,245,7,,,*35
$GPGGA,163928.814,,,,,0,00,,,M,0.0,M,,0000*5C
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,278,,,,*51
$GPRMC,163928.814,V,,,,,,,091111,,,N*4E
$PLSR,245,1,278,6,668,38,7,255,22,0,2*0C
$PLSR,245,2,640,3999,603,-4045,630,111,14,-612,4050*0D
$PLSR,245,7,,,*35
$GPGGA,163929.809,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,278,,,,*51
$GPRMC,163929.809,V,,,,,,,091111,,,N*43
$PLSR,245,1,278,6,668,3,18,256,22,0,2*09
$PLSR,245,2,556,4058,47,-4049,551,287,277,-85,4086*3C
$PLSR,245,7,,,*35
$GPGGA,163930.809,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,32,11,53,226,32,14,46,060,37,31,38,119,*7E
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,278,,,,*51
$GPRMC,163930.809,V,,,,,,,091111,,,N*4B
$PLSR,245,1,276,6,668,2,27,258,22,0,2*04
$PLSR,245,2,434,4073,31,-4052,428,426,419,-75,4074*35
$PLSR,245,7,,,*35
$GPGGA,163931.814,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,276,,,,*5F
$GPRMC,163931.814,V,,,,,,,091111,,,N*46
$PLSR,245,1,274,6,667,13,38,258,22,0,2*37
$PLSR,245,2,348,4075,203,-4040,315,596,577,-250,4047*30
$PLSR,245,7,,,*35
$GPGGA,163932.808,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,274,,,,*5D
$GPRMC,163932.808,V,,,,,,,091111,,,N*48
$PLSR,245,1,273,6,667,18,50,248,21,0,2*37
$PLSR,245,2,247,4077,290,-4011,185,807,789,-332,4005*3C
$PLSR,245,7,,,*35
$GPGGA,163933.814,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,273,,,,*5A
$GPRMC,163933.814,V,,,,,,,091111,,,N*44
$PLSR,245,1,273,6,667,21,43,253,22,0,2*36
$PLSR,245,2,300,4071,334,-4031,241,684,659,-378,4025*3B
$PLSR,245,7,,,*35
$GPGGA,163934.814,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,273,,,,*5A
$GPRMC,163934.814,V,,,,,,,091111,,,N*43
$PLSR,245,1,275,6,667,27,48,254,22,0,2*3A
$PLSR,245,2,308,4062,425,-4020,225,756,726,-473,4003*3B
$PLSR,245,7,,,*35
$GPGGA,163935.808,,,,,0,00,,,M,0.0,M,,0000*5D
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,35,11,53,226,25,14,46,060,36,31,38,119,*7E
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,275,,,,*5C
$GPRMC,163935.808,V,,,,,,,091111,,,N*4F
$PLSR,245,1,276,6,667,19,28,254,22,0,2*32
$PLSR,245,2,459,4059,303,-4049,424,447,410,-349,4061*34
$PLSR,245,7,,,*35
$GPGGA,163936.808,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,276,,,,*5F
$GPRMC,163936.808,V,,,,,,,091111,,,N*4C
$PLSR,245,1,276,6,667,24,29,251,22,0,2*38
$PLSR,245,2,427,4055,387,-4051,381,468,427,-431,4051*3E
$PLSR,245,7,,,*35
$GPGGA,163937.814,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,276,,,,*5F
$GPRMC,163937.814,V,,,,,,,091111,,,N*40
$PLSR,245,1,274,6,667,14,10,277,21,0,2*34
$PLSR,245,2,550,4053,206,-4057,543,147,117,-223,4088*30
$PLSR,245,7,,,*35
$GPGGA,163938.814,,,,,0,00,,,M,0.0,M,,0000*5D
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,274,,,,*5D
$GPRMC,163938.814,V,,,,,,,091111,,,N*4F
$PLSR,245,1,273,6,667,26,50,251,21,0,2*32
$PLSR,245,2,316,4062,414,-4011,231,796,765,-466,3997*3D
$PLSR,245,7,,,*35
$GPGGA,163939.808,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,273,,,,*5A
$GPRMC,163939.808,V,,,,,,,091111,,,N*43
$PLSR,245,1,275,6,666,32,39,252,22,0,2*3F
$PLSR,245,2,396,4044,510,-4036,316,621,573,-562,4016*3F
$PLSR,245,7,,,*35
$GPGGA,163940.814,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,37,11,53,226,30,14,46,060,36,31,38,119,*78
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,275,,,,*5C
$GPRMC,163940.814,V,,,,,,,091111,,,N*40
$PLSR,245,1,274,6,667,28,40,246,21,0,2*3C
$PLSR,245,2,362,4053,457,-4033,286,653,613,-507,4018*30
$PLSR,245,7,,,*35
$GPGGA,163941.808,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,274,,,,*5D
$GPRMC,163941.808,V,,,,,,,091111,,,N*4C
$PLSR,245,1,277,6,666,29,18,265,21,0,2*33
$PLSR,245,2,548,4034,444,-4054,520,275,213,-476,4062*39
$PLSR,245,7,,,*35
$GPGGA,163942.814,,,,,0,00,,,M,0.0,M,,0000*50
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,277,,,,*5E
$GPRMC,163942.814,V,,,,,,,091111,,,N*42
$PLSR,245,1,276,6,666,37,21,260,21,0,2*32
$PLSR,245,2,520,4021,575,-4055,477,326,252,-610,4042*30
$PLSR,245,7,,,*35
$GPGGA,163943.814,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,276,,,,*5F
$GPRMC,163943.814,V,,,,,,,091111,,,N*43
$PLSR,245,1,275,6,666,30,31,253,21,0,2*37
$PLSR,245,2,448,4042,478,-4047,390,494,440,-526,4038*3B
$PLSR,245,7,,,*35
$GPGGA,163944.808,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163944.808,V,,,,,,,091111,,,N*49
$PLSR,245,1,277,6,666,34,29,255,22,0,2*3D
$PLSR,245,2,484,4030,537,-4048,425,458,394,-584,4034*38
$PLSR,245,7,,,*35
$GPGGA,163945.814,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,39,11,53,226,30,14,46,060,36,31,38,119,*76
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,277,,,,*5E
$GPRMC,163945.814,V,,,,,,,091111,,,N*45
$PLSR,245,1,275,6,666,46,24,255,21,0,2*34
$PLSR,245,2,526,3997,724,-4052,465,377,285,-764,4014*38
$PLSR,245,7,,,*35
$GPGGA,163946.814,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163946.814,V,,,,,,,091111,,,N*46
$PLSR,245,1,275,6,666,41,24,255,21,0,2*33
$PLSR,245,2,501,4013,647,-4055,445,379,300,-686,4026*30
$PLSR,245,7,,,*35
$GPGGA,163947.808,,,,,0,00,,,M,0.0,M,,0000*58
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163947.808,V,,,,,,,091111,,,N*4A
$PLSR,245,1,275,6,666,35,28,255,21,0,2*3C
$PLSR,245,2,464,4031,553,-4052,405,443,380,-597,4034*31
$PLSR,245,7,,,*35
$GPGGA,163948.814,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163948.814,V,,,,,,,091111,,,N*48
$PLSR,245,1,275,6,667,24,30,261,21,0,2*33
$PLSR,245,2,446,4054,372,-4049,402,465,422,-418,4053*36
$PLSR,245,7,,,*35
$GPGGA,163949.814,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163949.814,V,,,,,,,091111,,,N*49
$PLSR,245,1,274,6,666,44,29,257,21,0,2*38
$PLSR,245,2,492,4008,687,-4050,419,452,371,-733,4013*30
$PLSR,245,7,,,*35
$GPGGA,163950.808,,,,,0,00,,,M,0.0,M,,0000*5E
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,39,11,53,226,26,14,46,060,36,31,38,119,*71
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,274,,,,*5D
$GPRMC,163950.808,V,,,,,,,091111,,,N*4C
$PLSR,245,1,274,6,666,33,32,254,22,0,2*32
$PLSR,245,2,408,4041,523,-4050,343,507,455,-567,4030*39
$PLSR,245,7,,,*35
$GPGGA,163951.808,,,,,0,00,,,M,0.0,M,,0000*5F
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,274,,,,*5D
$GPRMC,163951.808,V,,,,,,,091111,,,N*4D
$PLSR,245,1,275,6,666,32,32,252,22,0,2*34
$PLSR,245,2,412,4043,512,-4049,347,512,461,-557,4032*3C
$PLSR,245,7,,,*35
$GPGGA,163952.814,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163952.814,V,,,,,,,091111,,,N*43
$PLSR,245,1,275,6,665,31,29,257,21,0,2*38
$PLSR,245,2,448,4041,487,-4051,394,456,402,-531,4041*3B
$PLSR,245,7,,,*35
$GPGGA,163953.809,,,,,0,00,,,M,0.0,M,,0000*5C
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163953.809,V,,,,,,,091111,,,N*4E
$PLSR,245,1,273,6,666,28,35,252,21,0,2*3D
$PLSR,245,2,380,4052,448,-4045,317,560,519,-494,4032*32
$PLSR,245,7,,,*35
$GPGGA,163954.814,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,273,,,,*5A
$GPRMC,163954.814,V,,,,,,,091111,,,N*45
$PLSR,245,1,275,6,666,32,27,259,21,0,2*38
$PLSR,245,2,461,4038,499,-4053,410,421,364,-541,4044*3D
$PLSR,245,7,,,*35
$GPGGA,163955.814,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,40,11,53,226,34,14,46,060,35,31,38,119,*7F
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,275,,,,*5C
$GPRMC,163955.814,V,,,,,,,091111,,,N*44
$PLSR,245,1,275,6,665,31,32,253,21,0,2*36
$PLSR,245,2,418,4044,494,-4049,356,510,460,-540,4034*3D
$PLSR,245,7,,,*35
$GPGGA,163956.814,,,,,0,00,,,M,0.0,M,,0000*55
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163956.814,V,,,,,,,091111,,,N*47
$PLSR,245,1,275,6,666,29,29,256,21,0,2*33
$PLSR,245,2,430,4047,458,-4053,378,458,409,-501,4044*33
$PLSR,245,7,,,*35
$GPGGA,163957.809,,,,,0,00,,,M,0.0,M,,0000*58
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163957.809,V,,,,,,,091111,,,N*4A
$PLSR,245,1,275,6,666,34,31,255,21,0,2*35
$PLSR,245,2,399,4041,537,-4053,335,490,438,-579,4031*3B
$PLSR,245,7,,,*35
$GPGGA,163958.814,,,,,0,00,,,M,0.0,M,,0000*5B
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,275,,,,*5C
$GPRMC,163958.814,V,,,,,,,091111,,,N*49
$PLSR,245,1,274,6,666,31,33,258,21,0,2*3E
$PLSR,245,2,393,4047,484,-4050,331,516,470,-528,4034*3B
$PLSR,245,7,,,*35
$GPGGA,163959.809,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,274,,,,*5D
$GPRMC,163959.809,V,,,,,,,091111,,,N*44
$PLSR,245,1,273,6,666,26,45,256,21,0,2*30
$PLSR,245,2,263,4067,407,-4031,190,705,680,-445,4014*37
$PLSR,245,7,,,*35
$GPGGA,164000.814,,,,,0,00,,,M,0.0,M,,0000*58
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,32,78,287,40,11,53,226,33,14,46,060,35,31,38,119,*78
$GPGSV,3,2,12,20,31,278,,17,23,305,,18,21,094,,23,13,224,*77
$GPGSV,3,3,12,22,10,094,,05,05,026,,30,02,060,,12,,060,*70
$HCHDG,273,,,,*5A
$GPRMC,164000.814,V,,,,,,,091111,,,N*4A
$PLSR,245,1,272,6,665,22,49,254,21,0,2*38
$PLSR,245,2,212,4076,347,-4020,143,773,756,-380,4008*34
$PLSR,245,7,,,*35
$GPGGA,164001.814,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,272,,,,*5B
$GPRMC,164001.814,V,,,,,,,091111,,,N*4B
$PLSR,245,1,271,6,665,27,48,256,21,0,2*3D
$PLSR,245,2,229,4067,422,-4024,148,750,728,-456,4005*3E
$PLSR,245,7,,,*35
$GPGGA,164002.809,,,,,0,00,,,M,0.0,M,,0000*56
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,271,,,,*58
$GPRMC,164002.809,V,,,,,,,091111,,,N*44
$PLSR,245,1,271,6,665,32,46,255,21,0,2*34
$PLSR,245,2,245,4056,502,-4029,154,721,695,-536,4000*36
$PLSR,245,7,,,*35
$GPGGA,164003.808,4705.0163,N,00223.7087,E,0,03,,-48.0,M,48.0,M,,0000*64
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,271,,,,*58
$GPRMC,164003.808,V,4705.0163,N,00223.7087,E,,,091111,,,N*76
$PLSR,245,1,271,6,665,32,48,253,21,0,2*3C
$PLSR,245,2,221,4058,505,-4023,125,757,734,-536,3994*39
$PLSR,245,7,,,*35
$GPGGA,164004.713,4705.0163,N,00223.7089,E,0,03,,-48.0,M,48.0,M,,0000*68
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,271,,,,*58
$GPRMC,164004.713,V,4705.0163,N,00223.7089,E,,,091111,,,N*7A
$PLSR,245,1,272,6,665,27,49,251,21,0,2*38
$PLSR,245,2,229,4066,430,-4018,143,780,759,-465,3998*39
$PLSR,245,7,,,*35
$GPGGA,164005.814,4705.0159,N,00223.7241,E,1,03,6.1,-48.0,M,48.0,M,,0000*46
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$GPGSV,3,1,09,32,76,053,41,11,61,133,24,14,18,039,36,20,65,251,*75
$GPGSV,3,2,09,17,48,292,,23,30,176,,28,11,250,,31,10,079,*70
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,272,,,0.1,W*23
$GPRMC,164005.814,A,4705.0159,N,00223.7241,E,2.66,272,091111,0.1,W,A*37
$PLSR,245,1,271,6,665,27,54,254,21,0,2*32
$PLSR,245,2,196,4070,423,-4007,104,847,830,-454,3985*3C
$PLSR,245,7,123,-59,0*14
$GPGGA,164006.814,4705.0155,N,00223.7210,E,0,03,,-48.0,M,48.0,M,,0000*65
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,271,,,0.1,W*20
$GPRMC,164006.814,V,4705.0155,N,00223.7210,E,,271,091111,0.1,W,N*3B
$PLSR,245,1,272,6,665,29,52,251,21,0,2*3C
$PLSR,245,2,179,4065,460,-4011,83,825,809,-486,3985*06
$PLSR,245,7,,,*35
$GPGGA,164007.814,4705.0083,N,00223.7370,E,0,03,,-48.0,M,48.0,M,,0000*69
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,272,,,0.1,W*23
$GPRMC,164007.814,V,4705.0083,N,00223.7370,E,,272,091111,0.1,W,N*34
$PLSR,245,1,271,6,665,29,46,257,21,0,2*3C
$PLSR,245,2,238,4063,452,-4030,156,717,694,-486,4007*3D
$PLSR,245,7,,,*35
$GPGGA,164008.814,4704.9968,N,00223.7612,E,0,03,,-48.0,M,48.0,M,,0000*63
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,271,,,0.1,W*20
$GPRMC,164008.814,V,4704.9968,N,00223.7612,E,,271,091111,0.1,W,N*3D
$PLSR,245,1,274,6,665,46,38,256,21,0,2*38
$PLSR,245,2,343,4018,716,-4046,239,592,538,-756,3989*38
$PLSR,245,7,,,*35
$GPGGA,164009.814,4705.0185,N,00223.7169,E,0,03,,-48.0,M,48.0,M,,0000*6A
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,274,,,0.1,W*25
$GPRMC,164009.814,V,4705.0185,N,00223.7169,E,,274,091111,0.1,W,N*31
$PLSR,245,1,265,6,665,67,79,238,21,0,2*36
$PLSR,245,2,-71,3955,1057,-3881,-402,1246,1307,-979,3755*3C
$PLSR,245,7,,,*35
$GPGGA,164010.814,4705.0268,N,00223.6881,E,1,03,6.1,-48.0,M,48.0,M,,0000*44
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$GPGSV,3,1,09,32,76,053,41,11,61,133,31,14,18,039,36,20,65,251,*71
$GPGSV,3,2,09,17,48,292,,23,30,176,,28,11,250,,31,10,079,*70
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,265,,,0.1,W*25
$GPRMC,164010.814,A,4705.0268,N,00223.6881,E,0.22,265,091111,0.1,W,A*31
$PLSR,245,1,270,6,665,61,57,241,21,0,2*36
$PLSR,245,2,181,3972,979,-3992,-43,915,896,-994,3871*28
$PLSR,245,7,-6,-9,0*0A
$GPGGA,164011.814,4705.0336,N,00223.6688,E,1,03,6.1,-48.0,M,48.0,M,,0000*48
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$HCHDG,270,,,0.1,W*21
$GPRMC,164011.814,A,4705.0336,N,00223.6688,E,0.25,270,091111,0.1,W,A*3E
$PLSR,245,1,268,6,664,46,60,250,21,0,2*3F
$PLSR,245,2,139,4030,721,-3987,-30,941,931,-733,3921*2A
$PLSR,245,7,-12,3,0*18
$GPGGA,164012.814,4705.0349,N,00223.6674,E,1,03,6.1,-48.0,M,48.0,M,,0000*40
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$HCHDG,268,,,0.1,W*28
$GPRMC,164012.814,A,4705.0349,N,00223.6674,E,0.18,268,091111,0.1,W,A*31
$PLSR,245,1,270,6,665,32,61,258,21,0,2*3D
$PLSR,245,2,129,4063,490,-3988,13,935,925,-506,3957*04
$PLSR,245,7,-9,1,0*20
$GPGGA,164013.814,4705.0305,N,00223.6785,E,1,03,6.1,-48.0,M,48.0,M,,0000*46
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$HCHDG,270,,,0.1,W*21
$GPRMC,164013.814,A,4705.0305,N,00223.6785,E,0.06,270,091111,0.1,W,A*31
$PLSR,245,1,267,6,665,35,70,264,21,0,2*33
$PLSR,245,2,43,4061,520,-3960,-91,1041,1043,-513,3927*10
$PLSR,245,7,-3,1,0*2A
$GPGGA,164014.814,4705.0334,N,00223.6676,E,1,03,6.1,-48.0,M,48.0,M,,0000*4E
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$HCHDG,267,,,0.1,W*27
$GPRMC,164014.814,A,4705.0334,N,00223.6676,E,0.30,267,091111,0.1,W,A*3A
$PLSR,245,1,267,6,666,36,73,251,21,0,2*36
$PLSR,245,2,-48,4057,558,-3931,-202,1133,1149,-522,3897*07
$PLSR,245,7,-15,1,-1*31
$GPGGA,164015.814,4705.0328,N,00223.6696,E,1,03,6.1,-48.0,M,48.0,M,,0000*4C
$GPGSA,A,2,11,14,32,,,,,,,,,,6.8,6.1,3.2*3F
$GPGSV,3,1,09,32,76,053,33,11,61,133,39,14,18,039,36,20,65,251,*7C
$GPGSV,3,2,09,17,48,292,,23,30,176,,28,11,250,,31,10,079,*70
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,267,,,0.1,W*27
$GPRMC,164015.814,A,4705.0328,N,00223.6696,E,0.31,267,091111,0.1,W,A*39
$PLSR,245,1,265,6,666,22,86,238,21,0,2*34
$PLSR,245,2,-173,4076,354,-3844,-283,1386,1404,-273,3837*34
$PLSR,245,7,-15,2,0*1E
$GPGGA,164016.813,4705.0328,N,00223.6683,E,1,03,6.1,-48.0,M,48.0,M,,0000*4C
$GPGSA,A,2,11,14,32,,,,,,,,,,6.9,6.1,3.2*3E
$HCHDG,265,,,0.1,W*25
$GPRMC,164016.813,A,4705.0328,N,00223.6683,E,0.42,265,091111,0.1,W,A*3F
$PLSR,245,1,265,6,665,27,82,248,21,0,2*31
$PLSR,245,2,-157,4070,421,-3881,-281,1279,1300,-349,3868*3A
$PLSR,245,7,-21,0,-1*37
$GPGGA,164017.813,4705.0313,N,00223.6705,E,1,03,6.1,-48.0,M,48.0,M,,0000*4A
$GPGSA,A,2,11,14,32,,,,,,,,,,6.9,6.1,3.2*3E
$HCHDG,265,,,0.1,W*25
$GPRMC,164017.813,A,4705.0313,N,00223.6705,E,0.07,265,091111,0.1,W,A*38
$PLSR,245,1,265,6,665,-7,113,217,21,0,2*1D
$PLSR,245,2,-441,4069,-117,-3617,-337,1891,1869,306,3631*3C
$PLSR,245,7,3,-1,0*2A
$GPGGA,164018.813,4705.0305,N,00223.6713,E,0,03,,-48.0,M,48.0,M,,0000*6D
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,265,,,0.1,W*25
$GPRMC,164018.813,V,4705.0305,N,00223.6713,E,,265,091111,0.1,W,N*36
$PLSR,245,1,252,6,665,112,98,227,21,0,2*00
$PLSR,245,2,-452,3704,1690,-3652,-1120,1479,1800,-1343,3426*09
$PLSR,245,7,,,*35
$GPGGA,164019.813,4705.0289,N,00223.6749,E,0,03,,-48.0,M,48.0,M,,0000*66
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,252,,,0.1,W*21
$GPRMC,164019.813,V,4705.0289,N,00223.6749,E,,252,091111,0.1,W,N*39
$PLSR,245,1,206,6,665,164,121,164,21,0,2*37
$PLSR,245,2,-1401,2866,2568,-2292,-2816,1894,3091,-789,2568*01
$PLSR,245,7,,,*35
$GPGGA,164020.813,4705.0284,N,00223.6785,E,0,03,,-48.0,M,48.0,M,,0000*61
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$GPGSV,3,1,09,32,76,053,34,11,60,133,31,14,18,039,36,20,65,251,*72
$GPGSV,3,2,09,17,48,291,,23,30,176,,28,11,250,,31,10,078,*72
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,206,,,0.1,W*20
$GPRMC,164020.813,V,4705.0284,N,00223.6785,E,,206,091111,0.1,W,N*3F
$PLSR,245,1,261,6,664,100,80,226,21,0,2*0A
$PLSR,245,2,-205,3773,1576,-3827,-734,1261,1445,-1409,3563*33
$PLSR,245,7,,,*35
$GPGGA,164021.813,4705.0292,N,00223.6782,E,0,03,,-48.0,M,48.0,M,,0000*60
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,261,,,0.1,W*21
$GPRMC,164021.813,V,4705.0292,N,00223.6782,E,,261,091111,0.1,W,N*3F
$PLSR,245,1,267,6,665,121,55,216,21,0,2*05
$PLSR,245,2,57,3599,1954,-3977,-418,888,979,-1909,3489*2F
$PLSR,245,7,,,*35
$GPGGA,164022.813,4705.0320,N,00223.6730,E,0,03,,-48.0,M,48.0,M,,0000*62
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,267,,,0.1,W*27
$GPRMC,164022.813,V,4705.0320,N,00223.6730,E,,267,091111,0.1,W,N*3B
$PLSR,245,1,269,6,664,114,65,211,21,0,2*08
$PLSR,245,2,-74,3639,1879,-3904,-632,1071,1241,-1771,3478*0B
$PLSR,245,7,,,*35
$GPGGA,164023.813,4705.0327,N,00223.6726,E,0,03,,-48.0,M,48.0,M,,0000*63
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,269,,,0.1,W*29
$GPRMC,164023.813,V,4705.0327,N,00223.6726,E,,269,091111,0.1,W,N*34
$PLSR,245,1,265,6,664,92,56,252,21,0,2*3C
$PLSR,245,2,128,3856,1375,-4006,-164,837,842,-1370,3767*1D
$PLSR,245,7,,,*35
$GPGGA,164024.813,4705.0526,N,00223.6328,E,0,03,,-48.0,M,48.0,M,,0000*69
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,265,,,0.1,W*25
$GPRMC,164024.813,V,4705.0526,N,00223.6328,E,,265,091111,0.1,W,N*32
$PLSR,245,1,266,6,664,85,65,232,21,0,2*3F
$PLSR,245,2,-33,3863,1362,-3941,-401,1042,1116,-1302,3720*04
$PLSR,245,7,,,*35
$GPGGA,164025.813,4705.0539,N,00223.6305,E,0,03,,-48.0,M,48.0,M,,0000*69
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$GPGSV,3,1,09,32,76,053,38,11,60,133,16,14,18,039,36,20,65,251,*7B
$GPGSV,3,2,09,17,48,291,,23,30,176,,28,11,250,,31,10,078,*72
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,266,,,0.1,W*26
$GPRMC,164025.813,V,4705.0539,N,00223.6305,E,,266,091111,0.1,W,N*31
$PLSR,245,1,267,6,664,101,60,243,21,0,2*00
$PLSR,245,2,77,3798,1532,-3984,-286,910,951,-1507,3688*24
$PLSR,245,7,,,*35
$GPGGA,164026.814,4705.0419,N,00223.6552,E,0,03,,-48.0,M,48.0,M,,0000*6A
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,267,,,0.1,W*27
$GPRMC,164026.814,V,4705.0419,N,00223.6552,E,,267,091111,0.1,W,N*33
$PLSR,245,1,267,6,664,90,58,236,21,0,2*30
$PLSR,245,2,79,3839,1422,-3984,-256,916,947,-1400,3730*26
$PLSR,245,7,,,*35
$GPGGA,164027.814,4705.0382,N,00223.6631,E,0,03,,-48.0,M,48.0,M,,0000*68
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,267,,,0.1,W*27
$GPRMC,164027.814,V,4705.0382,N,00223.6631,E,,267,091111,0.1,W,N*31
$PLSR,245,1,267,6,664,91,54,244,21,0,2*38
$PLSR,245,2,124,3846,1401,-4007,-173,831,839,-1395,3758*1E
$PLSR,245,7,,,*35
$GPGGA,164028.814,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$HCHDG,267,,,0.1,W*27
$GPRMC,164028.814,V,,,,,,267,091111,0.1,W,N*0B
$PLSR,245,1,266,6,664,86,62,240,20,0,2*3F
$PLSR,245,2,32,3869,1342,-3969,-302,968,1012,-1307,3747*1F
$PLSR,245,7,,,*35
$GPGGA,164029.814,4705.0406,N,00223.6586,E,0,03,,-48.0,M,48.0,M,,0000*62
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,266,,,0.1,W*26
$GPRMC,164029.814,V,4705.0406,N,00223.6586,E,,266,091111,0.1,W,N*3A
$PLSR,245,1,266,6,664,89,62,244,21,0,2*35
$PLSR,245,2,52,3861,1365,-3974,-282,951,990,-1336,3743*2F
$PLSR,245,7,,,*35
$GPGGA,164030.814,4705.0389,N,00223.6622,E,0,03,,-48.0,M,48.0,M,,0000*67
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$GPGSV,3,1,09,32,76,053,39,11,60,133,25,14,18,039,36,20,65,251,*7A
$GPGSV,3,2,09,17,48,291,,23,30,176,,28,11,250,,31,10,078,*72
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,266,,,0.1,W*26
$GPRMC,164030.814,V,4705.0389,N,00223.6622,E,,266,091111,0.1,W,N*3F
$PLSR,245,1,266,6,664,100,58,231,21,0,2*0E
$PLSR,245,2,67,3775,1586,-3979,-314,919,967,-1555,3663*2F
$PLSR,245,7,,,*35
$GPGGA,164031.814,4705.0585,N,00223.6227,E,0,03,,-48.0,M,48.0,M,,0000*6D
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,266,,,0.1,W*26
$GPRMC,164031.814,V,4705.0585,N,00223.6227,E,,266,091111,0.1,W,N*35
$PLSR,245,1,265,6,665,100,67,243,21,0,2*05
$PLSR,245,2,-26,3807,1510,-3946,-428,1012,1098,-1448,3671*03
$PLSR,245,7,,,*35
$GPGGA,164032.814,4705.0499,N,00223.6400,E,0,03,,-48.0,M,48.0,M,,0000*61
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,265,,,0.1,W*25
$GPRMC,164032.814,V,4705.0499,N,00223.6400,E,,265,091111,0.1,W,N*3A
$PLSR,245,1,191,6,673,115,260,17,21,0,2*08
$PLSR,245,2,-1893,3233,1654,603,-1559,3739,3582,1971,244*37
$PLSR,245,7,,,*35
$GPGGA,164033.814,4705.0442,N,00223.6512,E,0,03,,-48.0,M,48.0,M,,0000*64
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,191,,,0.1,W*2D
$GPRMC,164033.814,V,4705.0442,N,00223.6512,E,,191,091111,0.1,W,N*37
$PLSR,245,1,181,6,673,182,171,122,21,0,2*33
$PLSR,245,2,-1789,2524,2682,-677,-3156,2520,3621,657,1798*12
$PLSR,245,7,,,*35
$GPGGA,164034.814,4705.0384,N,00223.6642,E,0,03,,-48.0,M,48.0,M,,0000*68
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,181,,,0.1,W*2C
$GPRMC,164034.814,V,4705.0384,N,00223.6642,E,,181,091111,0.1,W,N*3A
$PLSR,245,1,270,6,701,-25,-30,263,21,0,2*34
$PLSR,245,2,249,4069,-385,-4065,204,-462,-440,410,4051*3E
$PLSR,245,7,,,*35
$GPGGA,164035.814,4705.0310,N,00223.6917,E,0,03,,-48.0,M,48.0,M,,0000*6B
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$GPGSV,3,1,09,32,76,053,29,11,60,133,26,14,18,039,22,20,65,251,15*79
$GPGSV,3,2,09,17,48,291,,23,30,176,,28,11,250,,31,10,078,*72
$GPGSV,3,3,09,03,08,168,*44
$HCHDG,270,,,0.1,W*21
$GPRMC,164035.814,V,4705.0310,N,00223.6917,E,,270,091111,0.1,W,N*34


$PLSR,245,1,276,6,696,-10,-35,332,21,0,2*3B
$PLSR,245,2,430,4071,-122,-4052,415,-429,-413,165,4072*3D
$PLSR,245,7,,,*35
$GPGGA,164036.814,4705.0382,N,00223.6707,E,0,03,,-48.0,M,48.0,M,,0000*6C
$GPGSA,A,1,11,14,32,,,,,,,,,,,,*1A
$HCHDG,276,,,0.1,W*27
$GPRMC,164036.814,V,4705.0382,N,00223.6707,E,,276,091111,0.1,W,N*35

  */
