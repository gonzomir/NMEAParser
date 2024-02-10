#include <nmeaparser.h>

NMEAParser::NMEAParser() {
    lastPLSR2451.isValid = false;
	lastPLSR2451.isValid = false;
	lastPLSR2452.isValid = false;
	lastPLSR2457.isValid = false;
	lastGPGGA.isValid = false;
	lastGPGSA.isValid = false;
	lastGPGSV.isValid = false;
	lastHCHDG.isValid = false;
	lastGPRMC.isValid = false;
	lastGPGLL.isValid = false;
	lastGPVTG.isValid = false;
	lastGPTXT.isValid = false;
}

#ifdef ARDUINO
bool NMEAParser::dispatch(const String str) {
    return dispatch(str.c_str());
}
#else
bool NMEAParser::dispatch(const std::string str) {
    return dispatch(str.c_str());
}
#endif

bool NMEAParser::dispatch(const char *str) {
    if (!str[0]) {
        return false;
    }

    //check NMEA string type
    if (str[0] == '$') {
        //PLSR24r5X
        if (str[1] == 'P' && str[2] == 'L' && str[3] == 'S' && str[4] == 'R' && str[5] == ',' && str[6] == '2' && str[7] == '4' && str[8] == '5' && str[9] == ',') {
            if (str[10] == '1')
				return parsePLSR2451(str);
            if (str[10] == '2')
				return parsePLSR2452(str);
            if (str[10] == '7')
				return parsePLSR2457(str);
        } else if (str[1] == 'G' && (str[2] == 'P' || str[2] == 'A' || str[2] == 'L' || str[2] == 'B' || str[2] == 'I' || str[2] == 'Q')) {
            //GPGGA
            if      (str[3] == 'G' && str[4] == 'G' && str[5] == 'A')
				return parseGPGGA(str);
            //GPGSA
            else if (str[3] == 'G' && str[4] == 'S' && str[5] == 'A')
				return parseGPGSA(str);
            //GPGSV
            else if (str[3] == 'G' && str[4] == 'S' && str[5] == 'V')
				return parseGPGSV(str);
            //GPRMC
            else if (str[3] == 'R' && str[4] == 'M' && str[5] == 'C')
				return parseGPRMC(str);
            //GPVTG
            else if (str[3] == 'V' && str[4] == 'T' && str[5] == 'G')
				return parseGPVTG(str);
            //GPTXT
            else if (str[3] == 'T' && str[4] == 'X' && str[5] == 'T')
				return parseGPTXT(str);
            //GPGLL
            else if (str[3] == 'G' && str[4] == 'L' && str[5] == 'L')
				return parseGPGLL(str);
        }
        //HCHDG
        else if (str[1] == 'H' && str[2] == 'C' && str[3] == 'H' && str[4] == 'D' && str[5] == 'G')
			return parseHCHDG(str);
    }
    return false;
}

bool NMEAParser::parsePLSR2451(const char *str) {
    checksum=0;
    scanned = mysscanf(&lastPLSR2451.fieldValidity, str, "$PLSR,245,1,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &lastPLSR2451.direction,
            &lastPLSR2451.calibration_status,
            &lastPLSR2451.field_intensity,
            &lastPLSR2451.acceleration_x,
            &lastPLSR2451.acceleration_y,
            &lastPLSR2451.acceleration_z,
            &lastPLSR2451.temperature,
            &lastPLSR2451.mounting_mode,
            &lastPLSR2451.current_calibration,
            &checksum);

    lastPLSR2451.isValid = ((scanned == 10) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_PLSR2451;
    return lastPLSR2451.isValid;
}

bool NMEAParser::parsePLSR2452(const char *str) {
    checksum=0;
    scanned = mysscanf(&lastPLSR2452.fieldValidity, str, "$PLSR,245,2,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &lastPLSR2452.xx,
            &lastPLSR2452.yx,
            &lastPLSR2452.zx,
            &lastPLSR2452.xy,
            &lastPLSR2452.yy,
            &lastPLSR2452.zy,
            &lastPLSR2452.xz,
            &lastPLSR2452.yz,
            &lastPLSR2452.zz,
            &checksum);

    lastPLSR2452.isValid = ((scanned == 10) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_PLSR2452;
    return lastPLSR2452.isValid;
}

bool NMEAParser::parsePLSR2457(const char *str) {
    checksum=0;
    scanned = mysscanf(&lastPLSR2457.fieldValidity, str, "$PLSR,245,7,%d,%d,%d*%X",
           &lastPLSR2457.gps_speed_east,
           &lastPLSR2457.gps_speed_north,
           &lastPLSR2457.gps_speed_up,
           &checksum);

    lastPLSR2457.isValid = ((scanned == 4) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_PLSR2457;
    return lastPLSR2457.isValid;
}

bool NMEAParser::parseGPGGA(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPGGA.fieldValidity, str, "$G%cGGA,%s,%s,%c,%s,%c,%d,%d,%f,%f,%s,%f,%s,%f,%d*%X",
            lastGPGGA.gnss,
            lastGPGGA.utc_time,
            lastGPGGA.latitude,
            &lastGPGGA.north_south_indicator,
            lastGPGGA.longitude,
            &lastGPGGA.east_west_indicator,
            &lastGPGGA.position_fix,
            &lastGPGGA.satellites_used,
            &lastGPGGA.hdop,
            &lastGPGGA.msl_altitude,
            lastGPGGA.msl_unit,
            &lastGPGGA.geoid_separation,
            lastGPGGA.geoid_unit,
            &lastGPGGA.age_of_diff_corr,
            &lastGPGGA.station_id,
            &checksum);

    lastGPGGA.isValid = ((scanned == 15) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPGGA;
    return lastGPGGA.isValid;
}

bool NMEAParser::parseGPGSA(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPGSA.fieldValidity, str, "$G%cGSA,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f*%X",
            &lastGPGSA.gnss,
            &lastGPGSA.mode1,
            &lastGPGSA.mode2,
            &lastGPGSA.sat_channel_1,
            &lastGPGSA.sat_channel_2,
            &lastGPGSA.sat_channel_3,
            &lastGPGSA.sat_channel_4,
            &lastGPGSA.sat_channel_5,
            &lastGPGSA.sat_channel_6,
            &lastGPGSA.sat_channel_7,
            &lastGPGSA.sat_channel_8,
            &lastGPGSA.sat_channel_9,
            &lastGPGSA.sat_channel_10,
            &lastGPGSA.sat_channel_11,
            &lastGPGSA.sat_channel_12,
            &lastGPGSA.pdop,
            &lastGPGSA.hdop,
            &lastGPGSA.vdop,
            &checksum);

    lastGPGSA.isValid = ((scanned == 18) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPGSA;
    return lastGPGSA.isValid;
}

bool NMEAParser::parseGPGSV(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPGSV.fieldValidity, str, "$G%cGSV,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &lastGPGSV.gnss,
            &lastGPGSV.number_of_messages,
            &lastGPGSV.message_idx,
            &lastGPGSV.sats_in_view,
            &lastGPGSV.sat1_id,
            &lastGPGSV.sat1_elevation,
            &lastGPGSV.sat1_azimuth,
            &lastGPGSV.sat1_snr,
            &lastGPGSV.sat2_id,
            &lastGPGSV.sat2_elevation,
            &lastGPGSV.sat2_azimuth,
            &lastGPGSV.sat2_snr,
            &lastGPGSV.sat3_id,
            &lastGPGSV.sat3_elevation,
            &lastGPGSV.sat3_azimuth,
            &lastGPGSV.sat3_snr,
            &lastGPGSV.sat4_id,
            &lastGPGSV.sat4_elevation,
            &lastGPGSV.sat4_azimuth,
            &lastGPGSV.sat4_snr,
            &checksum);

    lastGPGSV.isValid = ((scanned == 20) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPGSV;
    return lastGPGSV.isValid;
}

bool NMEAParser::parseHCHDG(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastHCHDG.fieldValidity, str, "$HCHDG,%d,%f,%c,%f,%c*%X",
            &lastHCHDG.heading,
            &lastHCHDG.deviation,
            &lastHCHDG.dev_direction,
            &lastHCHDG.variation,
            &lastHCHDG.var_direction,
            &checksum);

    lastHCHDG.isValid = ((scanned == 6) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_HCHDG;
    return lastHCHDG.isValid;
}

bool NMEAParser::parseGPRMC(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPRMC.fieldValidity, str, "$G%cRMC,%s,%c,%s,%c,%s,%c,%f,%f,%s,%f,%c,%c*%X",
            &lastGPRMC.gnss,
            &lastGPRMC.utc_time,
            &lastGPRMC.status,
            lastGPRMC.latitude,
            &lastGPRMC.north_south_indicator,
            lastGPRMC.longitude,
            &lastGPRMC.east_west_indicator,
            &lastGPRMC.speed_over_ground,
            &lastGPRMC.course_over_ground,
            lastGPRMC.date,
            &lastGPRMC.magnetic_variation,
            &lastGPRMC.variation_sense,
            &lastGPRMC.mode,
            &checksum);

    lastGPRMC.isValid = ((scanned == 13) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPRMC;
    return lastGPRMC.isValid;
}

bool NMEAParser::parseGPGLL(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPGLL.fieldValidity, str, "$G%cGLL,%s,%c,%s,%c,%s,%c*%X",
            lastGPGLL.gnss,
            lastGPGLL.latitude,
            &lastGPGLL.north_south_indicator,
            lastGPGLL.longitude,
            &lastGPGLL.east_west_indicator,
            lastGPGLL.fix_time,
            &lastGPGLL.data_active,
            &checksum);

    lastGPGLL.isValid = ((scanned == 7) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPGLL;
    return lastGPGLL.isValid;
}

bool NMEAParser::parseGPVTG(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPVTG.fieldValidity, str, "$G%cVTG,%f,%c,%f,%c,%f,%c,%f,%c,%c*%X",
            &lastGPVTG.gnss,
            &lastGPVTG.measured_heading_1,
            &lastGPVTG.north_type_1,
            &lastGPVTG.measured_heading_2,
            &lastGPVTG.north_type_2,
            &lastGPVTG.ground_speed_1,
            &lastGPVTG.ground_speed_unit_1,
            &lastGPVTG.ground_speed_2,
            &lastGPVTG.ground_speed_unit_2,
            &lastGPVTG.mode,
            &checksum);

    lastGPVTG.isValid = ((scanned == 10) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPVTG;
    return lastGPVTG.isValid;
}

bool NMEAParser::parseGPTXT(const char *str) {
    checksum = 0;
    scanned = mysscanf(&lastGPTXT.fieldValidity, str, "$G%cTXT,%d,%d,%d,%s*%X",
            &lastGPTXT.gnss,
            &lastGPTXT.number_of_messages,
            &lastGPTXT.sentence_number,
            &lastGPTXT.text_identifier,
            lastGPTXT.message,
            &checksum);

    lastGPTXT.isValid = ((scanned == 5) && verifyChecksum(str));
    lastParsed = NMEAParser::TYPE_GPTXT;
    return lastGPTXT.isValid;
}

bool NMEAParser::verifyChecksum(const char *str) {
    //compute normal checksum
    char calculated_sum = generateChecksum(str);

    //retrieve checksum from the str
    //go to the '*'
    char *ptr = const_cast<char*>(str);
    while((*ptr) && (*ptr!='*')) ptr++;
	//skip the '*'
    ptr++;

    int16_t retrieved_sum = 0;
	//single char checksum
    if (!*(ptr+1))
        retrieved_sum = myatoh(*ptr);
	//two characters checksum
    else if (!*(ptr+2))
        retrieved_sum = (myatoh(*ptr)*16) + myatoh(*(ptr+1));


    return (calculated_sum == retrieved_sum);
}

char NMEAParser::generateChecksum(const char *str) {
    char *ptr = const_cast<char*>(str+1);
    char sum=0;
	//computes the XOR checksum
    while(*ptr && *ptr!='*') {
		sum ^= *(ptr++);
	}
    return sum;
}

int16_t NMEAParser::myatoh(char a) {
	//converts uppercase hex
    if (a >= 'A' && a <= 'F')
        return a - 'A' + 10;
	//converts lowecase hex
    else if (a >= 'a' && a <= 'f')
        return a - 'a' + 10;
	//converts digits
    else
        return a - '0';
}

char *NMEAParser::mystrncpy(char *dst, const char *src, int16_t n) {
    if (n!=0) {
        char *d = dst;
        const char *s = src;
        do {
            if ((*d++ = *s++) == 0) {
                while(--n) *d++ = 0;
                break;
            }
        } while(--n);
    }
    return dst;
}

int16_t NMEAParser::mystrlen(const char* str) {
    int16_t s = 0;
    while(str[s++]);
    return s;
}

int32_t NMEAParser::myatoi(const char *str) {
    int32_t ret = 0;
    int32_t sign = 1;
	//check for a minus sign
    if (*str == '-') {
        sign = -1;
        str++;
    }
	//process each digit
    while (IS_DIGIT(*str))
        ret = 10 * ret + *str++ - '0';
    return sign*ret;
}

float NMEAParser::myatof(const char *s) {
	//the resule
    float a = 0.0;
	// exponent, if we ever find one
    int16_t e = 0;
    int16_t c;

	//process digits until there is no more
    while ((c = *s++) != '\0' && IS_DIGIT(c)) {
        a = a*10.0 + (c - '0');
    }
	//process decimal point
    if (c == '.') {
        while ((c = *s++) != '\0' && IS_DIGIT(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
	//process exponents
    if (c == 'e' || c == 'E') {
        int16_t sign = 1;
        int16_t i = 0;
        c = *s++;
		//exponent sign
        if (c == '+') {
            c = *s++;
        } else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (IS_DIGIT(c)) {
            i = i*10 + (c - '0');
            c = *s++;
        }
        e += i*sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return a;
}

int16_t NMEAParser::mysscanf(int16_t *field_validity, const char *src, const char *format, ... ) {
    va_list ap;

    //pointers to types that can be converted
    int16_t conv = 0, index;
    char *a, *fp, *sp = const_cast<char*>(src), buf[128] = {'\0'};

    va_start(ap, format);

    //reset field validity flags
    *field_validity = 0;

    //for every character of the format string
    for (fp = const_cast<char*>(format); *fp != '\0'; fp++) {
        //if the format string and src string have the same character here, step forward into both
        if (*fp == *sp) {
            sp++;
            continue;
        }

        //compute the length of the next field. Stop at ' ', '\0', '*', or ','.
        for (index = 0; *sp != '\0' && /**sp != ' ' &&*/ *sp != ',' && *sp != '*'; index++)
            buf[index] = *sp++;

        //cut the string
        buf[index] = 0;

        //skip any space in the source string
        while (*sp == ' ') sp++;

        //go to the next '%' in the format string
        while (*fp != '%') fp++;

        if (*fp == '%') {
            int16_t *i;

            switch (*++fp) {

			//compute an int
            case 'i':
            case 'd': {
                i = va_arg(ap, int16_t *);
                if (!*buf) {
                    *i = 0;
                } else {
                    *i = myatoi(buf);
                    *field_validity = SET_BIT(*field_validity, conv);
                }
                break;
            }

			//compute a float
            case 'f': {
                float *f = va_arg(ap, float *);
                if (!*buf) {
                    *f = 0;
                } else {
                    *f = static_cast<float>(myatof(buf));
                    *field_validity = SET_BIT(*field_validity, conv);
                }
                break;
            }

			//compute a string
            case 's':
                a = va_arg(ap, char *);
                mystrncpy(a, buf, mystrlen(buf)+1);
                if (buf[0]) *field_validity = SET_BIT(*field_validity, conv);
                break;

            //compute a single char
            case 'c':
                a = va_arg(ap, char *);
                *a = buf[0];
                if (buf[0]) *field_validity = SET_BIT(*field_validity, conv);
                break;

			//compute an hexadecimal
            case 'X':
            case 'x':
                i = va_arg(ap, int16_t *);
                if (!*buf) {
                    *i = 0;
                } else {
                    if (!*(buf+1))
                        *i = myatoh(*buf);
                    else if (!*(buf+2))
                        *i = (myatoh(*buf)*16) + myatoh(*(buf+1));
                    else *i = -1;
                    *field_validity = SET_BIT(*field_validity, conv);
                }
                break;
            }
            conv++;
        }
    }
    va_end ( ap );
    return conv;
}
