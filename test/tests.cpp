#include <nmeaparser.h>
#include <unity.h>

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


void process() {
    UNITY_BEGIN();
    RUN_TEST(test_unknown_type);
    RUN_TEST(test_plsr2451);
    RUN_TEST(test_plsr2452);
    RUN_TEST(test_plsr2457);
    RUN_TEST(test_gpgll);
    RUN_TEST(test_gpvtg);
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
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
