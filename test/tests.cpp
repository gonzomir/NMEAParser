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

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_unknown_type);
    RUN_TEST(test_plsr2451);
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