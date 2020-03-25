#include <nmeaparser.h>
#include <unity.h>

const char * const strings[]  = {
    "$GPTXT,01,01,02,u-blox ag - www.u-blox.com*50",
    "$GPTXT,01,01,02,HW  UBX-G70xx   00070000 *77",
    "$GPTXT,01,01,02,ROM CORE 1.00 (59842) Jun 27 2012 17:43:52*59",
    "$GPTXT,01,01,02,PROTVER 14.00*1E",
    "$GPTXT,01,01,02,ANTSUPERV=AC SD PDoS SR*20",
    "$GPTXT,01,01,02,ANTSTATUS=OK*3B",
    "$GPTXT,01,01,02,LLC FFFFFFFF-FFFFFFFD-FFFFFFFF-FFFFFFFF-FFFFFFFD*2E",
    "$GPRMC,164047.00,A,4448.56047,N,00038.83771,W,0.097,,230320,,,A*61",
    "$GPVTG,,T,,M,0.097,N,0.179,K,A*22",
    "$GPGGA,164047.00,4448.56047,N,00038.83771,W,1,08,1.11,47.0,M,48.5,M,,*70",
    "$GPGSA,A,3,05,29,20,31,16,26,21,25,,,,,1.85,1.11,1.49*0A",
    "$GPGSV,3,1,10,05,07,035,38,16,41,305,22,18,57,074,42,20,13,143,27*7C",
    "$GPGSV,3,2,10,21,70,121,33,25,10,121,40,26,72,306,32,29,33,062,29*7E",
    "$GPGSV,3,3,10,31,43,208,33,39,32,146,29*74",
    "$GPGLL,4448.56047,N,00038.83771,W,164047.00,A,A*76",
    "$PLSR,245,1,215,0,1042,12,-16,260,20,0,2*2F",
    "$PLSR,245,2,-3345,2354,188,-2362,-3335,-251,7,-313,4084*16",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000034.106,,,,,0,00,,,M,0.0,M,,0000*56",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000034.106,V,,,,,,,150209,,,N*42",
    "$PLSR,245,1,215,0,1040,12,-15,261,19,0,2*25",
    "$PLSR,245,2,-3342,2360,187,-2368,-3333,-234,16,-299,4085*24",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000035.116,,,,,0,00,,,M,0.0,M,,0000*56",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$GPGSV,1,1,00*79",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000035.116,V,,,,,,,150209,,,N*42",
    "$PLSR,245,1,215,0,1041,12,-15,261,19,0,2*24",
    "$PLSR,245,2,-3349,2351,187,-2359,-3340,-234,17,-299,4085*2A",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000036.108,,,,,0,00,,,M,0.0,M,,0000*5A",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000036.108,V,,,,,,,150209,,,N*4E",
    "$PLSR,245,1,215,0,1040,13,-15,261,19,0,2*24",
    "$PLSR,245,2,-3347,2352,203,-2361,-3338,-234,30,-308,4084*21",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000037.107,,,,,0,00,,,M,0.0,M,,0000*54",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000037.107,V,,,,,,,150209,,,N*40",
    "$PLSR,245,1,215,0,1041,12,-16,260,20,0,2*2C",
    "$PLSR,245,2,-3344,2357,188,-2365,-3334,-251,7,-313,4084*12",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000038.117,,,,,0,00,,,M,0.0,M,,0000*5A",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000038.117,V,,,,,,,150209,,,N*4E",
    "$PLSR,245,1,215,0,1041,11,-16,260,20,0,2*2F",
    "$PLSR,245,2,-3340,2365,172,-2372,-3330,-251,-5,-304,4085*38",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000039.107,,,,,0,00,,,M,0.0,M,,0000*5A",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000039.107,V,,,,,,,150209,,,N*4E",
    "$PLSR,245,1,213,0,1052,-2,14,229,20,0,2*16",
    "$PLSR,245,2,-3403,2278,-35,-2276,-3396,250,108,227,4089*0F",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000040.105,,,,,0,00,,,M,0.0,M,,0000*56",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$GPGSV,1,1,00*79",
    "$HCHDG,213,,,,*5C",
    "$GPRMC,000040.105,V,,,,,,,150209,,,N*42",
    "$PLSR,245,1,215,0,1049,-81,5,295,20,0,2*16",
    "$PLSR,245,2,-2967,2607,-1084,-2717,-3063,66,-768,766,3949*15",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000041.115,,,,,0,00,,,M,0.0,M,,0000*56",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000041.115,V,,,,,,,150209,,,N*42",
    "$PLSR,245,1,208,0,1054,49,-13,235,19,0,2*25",
    "$PLSR,245,2,-3545,1873,834,-1951,-3593,-221,630,-588,4004*19",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000042.108,,,,,0,00,,,M,0.0,M,,0000*59",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,208,,,,*56",
    "$GPRMC,000042.108,V,,,,,,,150209,,,N*4D",
    "$PLSR,245,1,215,0,1027,25,-75,302,20,0,2*28",
    "$PLSR,245,2,-3129,2622,328,-2614,-2996,-984,-388,-961,3963*36",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000043.107,,,,,0,00,,,M,0.0,M,,0000*57",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,215,,,,*5A",
    "$GPRMC,000043.107,V,,,,,,,150209,,,N*43",
    "$PLSR,245,1,221,0,1048,13,-68,240,20,0,2*28",
    "$PLSR,245,2,-3060,2714,213,-2659,-2909,-1115,-586,-971,3936*00",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000044.118,,,,,0,00,,,M,0.0,M,,0000*5E",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,221,,,,*5D",
    "$GPRMC,000044.118,V,,,,,,,150209,,,N*4A",
    "$PLSR,245,1,224,0,1044,8,-69,266,20,0,2*1E",
    "$PLSR,245,2,-3045,2738,119,-2674,-2928,-1028,-601,-841,3963*0D",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000045.106,,,,,0,00,,,M,0.0,M,,0000*50",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$GPGSV,1,1,00*79",
    "$HCHDG,224,,,,*58",
    "$GPRMC,000045.106,V,,,,,,,150209,,,N*44",
    "$PLSR,245,1,226,0,1048,11,-78,245,20,0,2*29",
    "$PLSR,245,2,-2968,2817,175,-2726,-2794,-1241,-734,-1015,3900*3A",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000046.105,,,,,0,00,,,M,0.0,M,,0000*50",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,226,,,,*5A",
    "$GPRMC,000046.105,V,,,,,,,150209,,,N*44",
    "$PLSR,245,1,220,0,1029,26,-83,250,20,0,2*2C",
    "$PLSR,245,2,-3016,2741,402,-2709,-2790,-1284,-584,-1211,3868*32",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000047.116,,,,,0,00,,,M,0.0,M,,0000*53",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,220,,,,*5C",
    "$GPRMC,000047.116,V,,,,,,,150209,,,N*47",
    "$PLSR,245,1,217,0,1009,50,-76,233,20,0,2*24",
    "$PLSR,245,2,-3216,2399,818,-2532,-2968,-1244,-135,-1482,3815*31",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000048.106,,,,,0,00,,,M,0.0,M,,0000*5D",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,217,,,,*58",
    "$GPRMC,000048.106,V,,,,,,,150209,,,N*49",
    "$PLSR,245,1,188,0,1027,94,-21,382,20,0,2*2C",
    "$PLSR,245,2,-3617,1654,977,-1750,-3695,-218,793,-609,3971*1E",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000049.108,,,,,0,00,,,M,0.0,M,,0000*52",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,188,,,,*5D",
    "$GPRMC,000049.108,V,,,,,,,150209,,,N*46",
    "$PLSR,245,1,177,0,1052,163,50,230,20,0,2*34",
    "$PLSR,245,2,-3356,283,2332,156,-4030,715,2344,674,3291*31",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000050.116,,,,,0,00,,,M,0.0,M,,0000*55",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$GPGSV,1,1,00*79",
    "$HCHDG,177,,,,*5D",
    "$GPRMC,000050.116,V,,,,,,,150209,,,N*41",
    "$PLSR,245,1,163,0,1082,170,140,153,20,0,2*08",
    "$PLSR,245,2,-3023,950,2596,764,-3408,2138,2657,2062,2337*31",
    "$PLSR,245,7,,,*35",
    "$GPGGA,000051.106,,,,,0,00,,,M,0.0,M,,0000*55",
    "$GPGSA,A,1,,,,,,,,,,,,,,,*1E",
    "$HCHDG,163,,,,*58",
    "$GPRMC,000051.106,V,,,,,,,150209,,,N*41",
    "$PLSR,245,1,184,0,1034,66,-55,338,20,0,2*2D",
    "$PLSR,245,2,-3553,1886,775,-2006,-3512,-646,365,-939,3970*14",
    "$GPRMC,000048.106,V,,,,,,,150209,,,N*49",
    "$PLSR,245,1,188,0,1027,94,-21,382,20,0,2*2C",
    "$PLSR,245,2,-3617,1654,977,-1750,-3695,-218,793,-609,3971*1E",
};

#define NUMBER_OF_TURNS 1000

void test_unknown_type(void) {
    const String testSentence = "$PLSA";
    NMEAParser parser;
    parser.dispatch(testSentence);
    TEST_ASSERT_FALSE(parser.dispatch(testSentence));
}

void test_plsr2451(void) {
    const String testSentence = "$PLSR,245,1,213,0,1052,-2,14,229,20,0,2*16";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_plsr2451.isValid);
    TEST_ASSERT_EQUAL_INT16(213, parser.last_plsr2451.direction);
    TEST_ASSERT_EQUAL_INT16(0, parser.last_plsr2451.calibration_status);
    TEST_ASSERT_EQUAL_INT16(1052, parser.last_plsr2451.field_intensity);
    TEST_ASSERT_EQUAL_INT16(-2, parser.last_plsr2451.acceleration_x);
    TEST_ASSERT_EQUAL_INT16(14, parser.last_plsr2451.acceleration_y);
    TEST_ASSERT_EQUAL_INT16(229, parser.last_plsr2451.acceleration_z);
    TEST_ASSERT_EQUAL_INT16(20, parser.last_plsr2451.temperature);
    TEST_ASSERT_EQUAL_INT16(0, parser.last_plsr2451.mounting_mode);
    TEST_ASSERT_EQUAL_INT16(2, parser.last_plsr2451.current_calibration);
}

void test_plsr2452(void) {
    const String testSentence = "$PLSR,245,2,-3129,2622,328,-2614,-2996,-984,-388,-961,3963*36";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_plsr2452.isValid);
    TEST_ASSERT_EQUAL_INT16(-3129, parser.last_plsr2452.xx);
    TEST_ASSERT_EQUAL_INT16(2622, parser.last_plsr2452.yx);
    TEST_ASSERT_EQUAL_INT16(328, parser.last_plsr2452.zx);
    TEST_ASSERT_EQUAL_INT16(-2614, parser.last_plsr2452.xy);
    TEST_ASSERT_EQUAL_INT16(-2996, parser.last_plsr2452.yy);
    TEST_ASSERT_EQUAL_INT16(-984, parser.last_plsr2452.zy);
    TEST_ASSERT_EQUAL_INT16(-388, parser.last_plsr2452.xz);
    TEST_ASSERT_EQUAL_INT16(-961, parser.last_plsr2452.yz);
    TEST_ASSERT_EQUAL_INT16(3963, parser.last_plsr2452.zz);
}

void test_plsr2457(void) {
    const String testSentence = "$PLSR,245,7,-9,1,0*20";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_plsr2457.isValid);
    TEST_ASSERT_EQUAL_INT16(-9, parser.last_plsr2457.gps_speed_east);
    TEST_ASSERT_EQUAL_INT16(1, parser.last_plsr2457.gps_speed_north);
    TEST_ASSERT_EQUAL_INT16(0, parser.last_plsr2457.gps_speed_up);
}

void test_gpgll(void) {
    const String testSentence = "$GPGLL,4448.55381,N,00038.83314,W,134006.00,A,A*7B";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_gpgll.isValid);
    TEST_ASSERT_EQUAL_STRING("4448.55381", parser.last_gpgll.latitude);
    TEST_ASSERT(parser.last_gpgll.north_south_indicator == 'N');
    TEST_ASSERT_EQUAL_STRING("00038.83314", parser.last_gpgll.longitude);
    TEST_ASSERT(parser.last_gpgll.east_west_indicator == 'W');
    TEST_ASSERT_EQUAL_STRING("134006.00", parser.last_gpgll.fix_time);
    TEST_ASSERT(parser.last_gpgll.data_active == 'A');
}

void test_gpvtg(void) {
    const String testSentence = "$GPVTG,,T,,M,0.097,N,0.179,K,A*22";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_gpvtg.isValid);
    TEST_ASSERT_EQUAL_FLOAT(0, parser.last_gpvtg.measured_heading_1);
    TEST_ASSERT_EQUAL_CHAR('T', parser.last_gpvtg.north_type_1);
    TEST_ASSERT_EQUAL_FLOAT(0, parser.last_gpvtg.measured_heading_2);
    TEST_ASSERT_EQUAL_CHAR('M', parser.last_gpvtg.north_type_2);
    TEST_ASSERT_EQUAL_FLOAT(0.097, parser.last_gpvtg.ground_speed_1);
    TEST_ASSERT_EQUAL_CHAR('N', parser.last_gpvtg.ground_speed_unit_1);
    TEST_ASSERT_EQUAL_FLOAT(0.179, parser.last_gpvtg.ground_speed_2);
    TEST_ASSERT_EQUAL_CHAR('K', parser.last_gpvtg.ground_speed_unit_2);
    TEST_ASSERT_EQUAL_CHAR('A', parser.last_gpvtg.mode);
}

void test_gptxt(void) {
    const String testSentence = "$GPTXT,01,01,02,ROM CORE 1.00 (59842) Jun 27 2012 17:43:52*59";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_gptxt.isValid);
    TEST_ASSERT_EQUAL_INT8(1, parser.last_gptxt.number_of_messages);
    TEST_ASSERT_EQUAL_INT8(1, parser.last_gptxt.sentence_number);
    TEST_ASSERT_EQUAL_INT8(2, parser.last_gptxt.text_identifier);
    TEST_ASSERT_EQUAL_STRING(const_cast<const char*>(parser.last_gptxt.message), "ROM CORE 1.00 (59842) Jun 27 2012 17:43:52");
}

void test_gpgsv(void) {
    const String testSentence = "$GPGSV,3,1,10,05,07,035,38,16,41,305,22,18,57,074,42,20,13,143,27*7C";
    NMEAParser parser;
    TEST_ASSERT_TRUE(parser.dispatch(testSentence));
    TEST_ASSERT_TRUE(parser.last_gpgsv.isValid);

    TEST_ASSERT_EQUAL_INT16(3, parser.last_gpgsv.number_of_messages);
    TEST_ASSERT_EQUAL_INT16(1, parser.last_gpgsv.message_idx);
    TEST_ASSERT_EQUAL_INT16(10, parser.last_gpgsv.sats_in_view);
    TEST_ASSERT_EQUAL_INT16(5, parser.last_gpgsv.sat1_id);
    TEST_ASSERT_EQUAL_INT16(7, parser.last_gpgsv.sat1_elevation);
    TEST_ASSERT_EQUAL_INT16(35, parser.last_gpgsv.sat1_azimuth);
    TEST_ASSERT_EQUAL_INT16(38, parser.last_gpgsv.sat1_snr);
    TEST_ASSERT_EQUAL_INT16(16, parser.last_gpgsv.sat2_id);
    TEST_ASSERT_EQUAL_INT16(41, parser.last_gpgsv.sat2_elevation);
    TEST_ASSERT_EQUAL_INT16(305, parser.last_gpgsv.sat2_azimuth);
    TEST_ASSERT_EQUAL_INT16(22, parser.last_gpgsv.sat2_snr);
    TEST_ASSERT_EQUAL_INT16(18, parser.last_gpgsv.sat3_id);
    TEST_ASSERT_EQUAL_INT16(57, parser.last_gpgsv.sat3_elevation);
    TEST_ASSERT_EQUAL_INT16(74, parser.last_gpgsv.sat3_azimuth);
    TEST_ASSERT_EQUAL_INT16(42, parser.last_gpgsv.sat3_snr);
    TEST_ASSERT_EQUAL_INT16(20, parser.last_gpgsv.sat4_id);
    TEST_ASSERT_EQUAL_INT16(13, parser.last_gpgsv.sat4_elevation);
    TEST_ASSERT_EQUAL_INT16(143, parser.last_gpgsv.sat4_azimuth);
    TEST_ASSERT_EQUAL_INT16(27, parser.last_gpgsv.sat4_snr);
}

void performance(void) {
    char buffer[200];

    NMEAParser parser;
    uint16_t n = sizeof(strings)/sizeof(strings[0]);
    uint16_t nfailed = 0;

    sprintf(buffer, "Parsing %d NMEA sentences", n * NUMBER_OF_TURNS);
    TEST_MESSAGE(buffer);

    uint32_t before = millis();

    for (uint16_t turn=0; turn<NUMBER_OF_TURNS; turn++) { 
        for (uint16_t i=0; i<n; i++) {
            if (!parser.dispatch(strings[i])) 
                nfailed++;
        }
    }

    uint32_t after = millis();

    sprintf(buffer, "Done in %d ms, failed %d times", after - before, nfailed);
    TEST_MESSAGE(buffer);

    //nothing failed, so PIO unit tests framework assumes that test succeeded.
}


void process() {
    UNITY_BEGIN();
    RUN_TEST(test_unknown_type);
    RUN_TEST(test_plsr2451);
    RUN_TEST(test_plsr2452);
    RUN_TEST(test_plsr2457);
    RUN_TEST(test_gpgll);
    RUN_TEST(test_gpvtg);
    RUN_TEST(test_gptxt);
    RUN_TEST(test_gpgsv);
    RUN_TEST(performance);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    process();
}

void loop() {
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
