#include <Arduino.h>
#include <nmeaparser.h>

const char * const strings[]  = {
    //don't bother google-mapping these coordinates, this is not the place I live <3
    "$GPRMC,164047.00,A,4448.56047,N,00038.83771,W,0.097,,230320,,,A*61",
    "$GPGSV,3,1,10,05,07,035,38,16,41,305,22,18,57,074,42,20,13,143,27*7C",
    "$GPGSV,3,2,10,21,70,121,33,25,10,121,40,26,72,306,32,29,33,062,29*7E",
    "$GPGSV,3,3,10,31,43,208,33,39,32,146,29*74",
    "$DUMMY,THIS,SENTENCE,WILL,NOT,PARSE*18"
};

void setup() {
    Serial.begin(9600);

    NMEAParser parser;

    uint16_t n = sizeof(strings)/sizeof(strings[0]);

    char buffer[200];
    sprintf(buffer, "Parsing %d NMEA sentences", n);
    Serial.println(buffer);

    for (uint16_t i=0; i<n; i++) {
        // Feed the parser with a sentence
        if (parser.dispatch(strings[i])) {
            // gest the last parsed sentence type
            switch(parser.getLastProcessedType()) {
                // is it a GPRMC sentence?
                case NMEAParser::TYPE_GPRMC:
                    //show some of the sentence's data
                    Serial.println("\nFound GPRMC sentence");
                    sprintf(buffer, " I'm at %s %c / %s %c", parser.last_gprmc.latitude, parser.last_gprmc.north_south_indicator, parser.last_gprmc.longitude, parser.last_gprmc.east_west_indicator);
                    Serial.println(buffer);
                    break;
                case NMEAParser::TYPE_GPGSV:
                    //show some of the sentence's data
                    //GPGSV sentences most of the time comes in pair, sometimes more, as each of these can give info about 4 sats at a time.
                    //So let's display this header only once
                    if (parser.last_gpgsv.message_idx == 1) { 
                        Serial.println("\nFound GPGSV sentence");
                        sprintf(buffer, " GPS can see %d satellites:", parser.last_gpgsv.sats_in_view);
                        Serial.println(buffer);
                    }
                    sprintf(buffer, " Satellite ID=%X, elevation=%d, azimuth=%d, signal/noise ratio: %d", parser.last_gpgsv.sat1_id, parser.last_gpgsv.sat1_elevation, parser.last_gpgsv.sat1_azimuth, parser.last_gpgsv.sat1_snr);
                    Serial.println(buffer);
                    sprintf(buffer, " Satellite ID=%X, elevation=%d, azimuth=%d, signal/noise ratio: %d", parser.last_gpgsv.sat2_id, parser.last_gpgsv.sat2_elevation, parser.last_gpgsv.sat2_azimuth, parser.last_gpgsv.sat2_snr);
                    Serial.println(buffer);
                    sprintf(buffer, " Satellite ID=%X, elevation=%d, azimuth=%d, signal/noise ratio: %d", parser.last_gpgsv.sat3_id, parser.last_gpgsv.sat3_elevation, parser.last_gpgsv.sat3_azimuth, parser.last_gpgsv.sat3_snr);
                    Serial.println(buffer);
                    sprintf(buffer, " Satellite ID=%X, elevation=%d, azimuth=%d, signal/noise ratio: %d", parser.last_gpgsv.sat4_id, parser.last_gpgsv.sat4_elevation, parser.last_gpgsv.sat4_azimuth, parser.last_gpgsv.sat4_snr);
                    Serial.println(buffer);
                    break;
                case NMEAParser::UNKNOWN:
                case NMEAParser::TYPE_PLSR2451:
                case NMEAParser::TYPE_PLSR2452:
                case NMEAParser::TYPE_PLSR2457:
                case NMEAParser::TYPE_GPGGA:
                case NMEAParser::TYPE_GPGSA:
                case NMEAParser::TYPE_HCHDG:
                case NMEAParser::TYPE_GPGLL:
                case NMEAParser::TYPE_GPVTG:
                case NMEAParser::TYPE_GPTXT:
                    break;
            }
        } else {
            Serial.print("\nCouldn't parse \"");
            Serial.print(strings[i]);
            Serial.print("\"");
        } 
    }
}

void loop() {
}