#include <nmeaparser.h>

NMEAParser::NMEAParser() {
    last_plsr2451.isValid = false;
}

bool NMEAParser::dispatch(const String str) {
    return dispatch(str.c_str());
}

bool NMEAParser::dispatch(const char *str) {
    if (!str[0]) return false;

    //check NMEA string type
    if (str[0] == '$') {
        //PLSR245X
        if (str[1] == 'P' && str[2] == 'L' && str[3] == 'S' && str[4] == 'R' && str[5] == ',' && str[6] == '2' && str[7] == '4' && str[8] == '5' && str[9] == ',') {
            if (str[10] == '1') return parse_plsr2451(str);
            if (str[10] == '2') return parse_plsr2452(str);
            if (str[10] == '7') return parse_plsr2457(str);
        }
        //GPGGA
        else if (str[1] == 'G' && str[2] == 'P' && str[3] == 'G' && str[4] == 'G' && str[5] == 'A') return parse_gpgga(str);
        //GPGSA
        else if (str[1] == 'G' && str[2] == 'P' && str[3] == 'G' && str[4] == 'S' && str[5] == 'A') return parse_gpgsa(str);
        //GPGSV
        else if (str[1] == 'G' && str[2] == 'P' && str[3] == 'G' && str[4] == 'S' && str[5] == 'V') return parse_gpgsv(str);
        //HCHDG
        else if (str[1] == 'H' && str[2] == 'C' && str[3] == 'H' && str[4] == 'D' && str[5] == 'G') return parse_hchdg(str);
        //GPRMC
        else if (str[1] == 'G' && str[2] == 'P' && str[3] == 'R' && str[4] == 'M' && str[5] == 'C') return parse_gprmc(str);
    }
    return false;
}

bool NMEAParser::parse_plsr2451(const char *str) {
    checksum=0;
    scanned = my_sscanf(&last_plsr2451.fieldValidity, str, "$PLSR,245,1,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &last_plsr2451.direction,
            &last_plsr2451.calibration_status,
            &last_plsr2451.field_intensity,
            &last_plsr2451.acceleration_x,
            &last_plsr2451.acceleration_y,
            &last_plsr2451.acceleration_z,
            &last_plsr2451.temperature,
            &last_plsr2451.mounting_mode,
            &last_plsr2451.current_calibration,
            &checksum);

    last_plsr2451.isValid = ((scanned == 10) && check_checksum(str));
    last_processed = NMEAParser::TYPE_PLSR2451;
    return last_plsr2451.isValid;
}

bool NMEAParser::parse_plsr2452(const char *str) {
    checksum=0;
    scanned = my_sscanf(&last_plsr2452.fieldValidity, str, "$PLSR,245,2,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &last_plsr2452.xx,
            &last_plsr2452.yx,
            &last_plsr2452.zx,
            &last_plsr2452.xy,
            &last_plsr2452.yy,
            &last_plsr2452.zy,
            &last_plsr2452.xz,
            &last_plsr2452.yz,
            &last_plsr2452.zz,
            &checksum);

    last_plsr2452.isValid = ((scanned == 10) && check_checksum(str));
    last_processed = NMEAParser::TYPE_PLSR2452;
    return last_plsr2452.isValid;
}

bool NMEAParser::parse_plsr2457(const char *str) {
    checksum=0;
    scanned = my_sscanf(&last_plsr2457.fieldValidity, str, "$PLSR,245,7,%d,%d,%d*%X",
           &last_plsr2457.gps_speed_east,
           &last_plsr2457.gps_speed_north,
           &last_plsr2457.gps_speed_up,
           &checksum);

    last_plsr2457.isValid = ((scanned == 4) && check_checksum(str));
    last_processed = NMEAParser::TYPE_PLSR2457;
    return last_plsr2457.isValid;
}

bool NMEAParser::parse_gpgga(const char *str) {
    checksum = 0;
    scanned = my_sscanf(&last_gpgga.fieldValidity, str, "$GPGGA,%s,%s,%c,%s,%c,%d,%d,%f,%f,%s,%f,%s,%f,%d*%X",
            last_gpgga.utc_time,
            last_gpgga.latitude,
            &last_gpgga.north_south_indicator,
            last_gpgga.longitude,
            &last_gpgga.east_west_indicator,
            &last_gpgga.position_fix,
            &last_gpgga.satellites_used,
            &last_gpgga.hdop,
            &last_gpgga.msl_altitude,
            last_gpgga.msl_unit,
            &last_gpgga.geoid_separation,
            last_gpgga.geoid_unit,
            &last_gpgga.age_of_diff_corr,
            &last_gpgga.station_id,
            &checksum);

    last_gpgga.isValid = ((scanned == 15) && check_checksum(str));
    last_processed = NMEAParser::TYPE_GPGGA;
    return last_gpgga.isValid;
}

bool NMEAParser::parse_gpgsa(const char *str) {
    checksum = 0;
    scanned = my_sscanf(&last_gpgsa.fieldValidity, str, "$GPGSA,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f*%X",
            &last_gpgsa.mode1,
            &last_gpgsa.mode2,
            &last_gpgsa.sat_channel_1,
            &last_gpgsa.sat_channel_2,
            &last_gpgsa.sat_channel_3,
            &last_gpgsa.sat_channel_4,
            &last_gpgsa.sat_channel_5,
            &last_gpgsa.sat_channel_6,
            &last_gpgsa.sat_channel_7,
            &last_gpgsa.sat_channel_8,
            &last_gpgsa.sat_channel_9,
            &last_gpgsa.sat_channel_10,
            &last_gpgsa.sat_channel_11,
            &last_gpgsa.sat_channel_12,
            &last_gpgsa.pdop,
            &last_gpgsa.hdop,
            &last_gpgsa.vdop,
            &checksum);

    last_gpgsa.isValid = ((scanned == 18) && check_checksum(str));
    last_processed = NMEAParser::TYPE_GPGSA;
    return last_gpgsa.isValid;
}

bool NMEAParser::parse_gpgsv(const char *str) {
    checksum = 0;
    scanned = my_sscanf(&last_gpgsv.fieldValidity, str, "$GPGSV,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d*%X",
            &last_gpgsv.number_of_messages,
            &last_gpgsv.message_idx,
            &last_gpgsv.sats_in_view,
            &last_gpgsv.sat1_id,
            &last_gpgsv.sat1_elevation,
            &last_gpgsv.sat1_azimuth,
            &last_gpgsv.sat1_snr,
            &last_gpgsv.sat2_id,
            &last_gpgsv.sat2_elevation,
            &last_gpgsv.sat2_azimuth,
            &last_gpgsv.sat2_snr,
            &last_gpgsv.sat3_id,
            &last_gpgsv.sat3_elevation,
            &last_gpgsv.sat3_azimuth,
            &last_gpgsv.sat3_snr,
            &last_gpgsv.sat4_id,
            &last_gpgsv.sat4_elevation,
            &last_gpgsv.sat4_azimuth,
            &last_gpgsv.sat4_snr,
            &checksum);

    last_gpgsv.isValid = ((scanned == 20) && check_checksum(str));
    last_processed = NMEAParser::TYPE_GPGSV;
    return last_gpgsv.isValid;
}

bool NMEAParser::parse_hchdg(const char *str) {
    checksum = 0;
    scanned = my_sscanf(&last_hchdg.fieldValidity, str, "$HCHDG,%d,%f,%c,%f,%c*%X",
            &last_hchdg.heading,
            &last_hchdg.deviation,
            &last_hchdg.dev_direction,
            &last_hchdg.variation,
            &last_hchdg.var_direction,
            &checksum);

    last_hchdg.isValid = ((scanned == 6) && check_checksum(str));
    last_processed = NMEAParser::TYPE_HCHDG;
    return last_hchdg.isValid;
}

bool NMEAParser::parse_gprmc(const char *str) {
    checksum = 0;
    scanned = my_sscanf(&last_gprmc.fieldValidity, str, "$GPRMC,%s,%c,%s,%c,%s,%c,%f,%f,%s,%f,%c,%c*%X",
            &last_gprmc.utc_time,
            &last_gprmc.status,
            last_gprmc.latitude,
            &last_gprmc.north_south_indicator,
            last_gprmc.longitude,
            &last_gprmc.east_west_indicator,
            &last_gprmc.speed_over_ground,
            &last_gprmc.course_over_ground,
            last_gprmc.date,
            &last_gprmc.magnetic_variation,
            &last_gprmc.variation_sense,
            &last_gprmc.mode,
            &checksum);

    last_gprmc.isValid = ((scanned == 13) && check_checksum(str));
    last_processed = NMEAParser::TYPE_GPRMC;
    return last_gprmc.isValid;
}

bool NMEAParser::check_checksum(const char *str) {
    //compute normal checksum
    char calculated_sum = generate_checksum(str);

    //retrieve checksum from the str
    //go to the '*'
    char *ptr = (char*)str;
    while((*ptr) && (*ptr!='*')) ptr++;
    ptr++;

    int16_t retrieved_sum = 0;
    if (!*(ptr+1))
        retrieved_sum = my_atoh(*ptr);
    else if (!*(ptr+2))
        retrieved_sum = (my_atoh(*ptr)*16) + my_atoh(*(ptr+1));


    return (calculated_sum == retrieved_sum);
}

char NMEAParser::generate_checksum(const char *str) {
    char *ptr = (char*)(str+1);
    char sum=0;
    while(*ptr && *ptr!='*') sum ^= *(ptr++);
    return sum;
}

int16_t NMEAParser::my_atoh(char a) {
    if (a >= 'A' && a <= 'F')
        return a - 'A' + 10;
    else if (a >= 'a' && a <= 'f')
        return a - 'a' + 10;
    else
        return a - '0';
}

char *NMEAParser::my_strncpy(char *dst, const char *src, int16_t n) {
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

int16_t NMEAParser::my_strlen(const char* str) {
    int16_t s = 0;
    while(str[s++]);
    return s;
}

int32_t NMEAParser::my_atoi(const char *str) {
    int32_t ret = 0;
    int32_t sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (IS_DIGIT(*str))
        ret = 10 * ret + *str++ - '0';
    return sign*ret;
}

float NMEAParser::my_atof(const char *s) {
    float a = 0.0;
    int16_t e = 0;
    int16_t c;
    while ((c = *s++) != '\0' && IS_DIGIT(c)) {
        a = a*10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && IS_DIGIT(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
    if (c == 'e' || c == 'E') {
        int16_t sign = 1;
        int16_t i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
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

int16_t NMEAParser::my_sscanf(int16_t *field_validity, const char *src, const char *format, ... ) {
    va_list ap;

    //pointers to types that can be converted
    int16_t conv = 0, index;
    char *a, *fp, *sp = (char*)src, buf[128] = {'\0'};

    va_start(ap, format);

    //reset field validity flags
    *field_validity = 0;

    //for every character of the format string
    for (fp = (char*)format; *fp != '\0'; fp++) {
        //if the format string and src string have the same character here, step forward into both
        if (*fp == *sp) {
            sp++;
            continue;
        }

        //compute the length of the next field. Stop at ' ', '\0', '*', or ','.
        for (index = 0; *sp != '\0' && *sp != ' ' && *sp != ',' && *sp != '*'; index++)
            buf[index] = *sp++;

        //cut the string
        buf[index] = 0;

        //skip any space in the source string
        while (*sp == ' ') sp++;

        //go to the next '%' in the format string
        while (*fp != '%') fp++;

        if (*fp == '%') {
            switch (*++fp) {
            //compute an int
            case 'i':
            case 'd':
                int16_t *i = va_arg(ap, int16_t *);
                if (!*buf) *i = 0;
                else {
                    *i = my_atoi(buf);
                    *field_validity = SET_BIT(*field_validity, conv);
                }
                break;
            //compute a float
            case 'f':
                float *f = va_arg(ap, float *);
                if (!*buf) *f = 0;
                else {
                    *f = (float)my_atof(buf);
                    *field_validity = SET_BIT(*field_validity, conv);
                }
                break;
            //compute a string
            case 's':
                a = va_arg(ap, char *);
                my_strncpy(a, buf, my_strlen(buf)+1);
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
                if (!*buf) *i = 0;
                else {
                    if (!*(buf+1))
                        *i = my_atoh(*buf);
                    else if (!*(buf+2))
                        *i = (my_atoh(*buf)*16) + my_atoh(*(buf+1));
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