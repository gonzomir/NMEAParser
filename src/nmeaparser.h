#pragma once

#ifdef ARDUINO
	#include <Arduino.h>
#else
	#include <stdint.h>
	#include <string>
#endif
#include <stdarg.h>

/*
	Checksum = 8bit exclusive OR of all characters, excluding '$' and '*'
*/

/**
 * PLSR Compass measurement report 1: calibration and acceleration.
 * Rate: default 1Hz, maximum 5Hz
 */
struct PLSR2451 {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	int16_t direction;				/**< Direction (degree. Magnetic direction, 0-360, 0=north)*/
	int16_t calibration_status;		/**< */
	int16_t field_intensity;		/**< */
	int16_t acceleration_x;			/**< Acceleration X (-512 to 511 -> -2G to 2G)*/
	int16_t acceleration_y;			/**< Acceleration Y (-512 to 511 -> -2G to 2G)*/
	int16_t acceleration_z;			/**< Acceleration Z (-512 to 511 -> -2G to 2G)*/
	int16_t temperature;			/**< Temperature (Celsius)*/
	int16_t mounting_mode;			/**< */
	int16_t current_calibration;	/**< Current Calibration (non zero: valid, 0=not valid)*/
};

/*
	$PLSR,245,2
	PLSR Compass measurement report 2: attitude. The PLSR compass measurement report 2 contains
	a set of the attitude vectors, each row of the matrix means attitude vectors and it is
	normalized with 0x1000.
	Rate: 1hz, maximum 5Hz
	Sentence ID		 $PLSR,245,2,
	Xx				  -3520 (X acceleration on X axis)
	Yx				  2066 (Y acceleration on X axis)
	Zx				  340 (...)
	Xy				  -2087
	Yy				  -3516
	Zy				  -232
	Xz				  174
	Yz				  -372
	Zz				  4075
	Checksum			*11
*/
struct PLSR2452 {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	int16_t xx;						/**< */
	int16_t yx;						/**< */
	int16_t zx;						/**< */
	int16_t xy;						/**< */
	int16_t yy;						/**< */
	int16_t zy;						/**< */
	int16_t xz;						/**< */
	int16_t yz;						/**< */
	int16_t zz;						/**< */
};

/*
	$PLSR,245,7
	3D GPS Speed output (ECEF coordinate)
	Sentence ID		 $PLSR,245,7,
	GPS Speed (east)	11
	GPS Speed (north)   -6
	GPS Speed (up)	  9
	Checksum			 *17
*/
struct PLSR2457 {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	int16_t gps_speed_east;			/**< */
	int16_t gps_speed_north;		/**< */
	int16_t gps_speed_up;			/**< */
};

/**
 * GGA - essential fix data which provide 3D location and accuracy data.
 * Example: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 * If the height of geoid is missing then the altitude should be suspect. Some non-standard implementations report altitude with respect to the ellipsoid rather than geoid altitude. Some units do not report negative altitudes at all. This is the only sentence that reports altitude.
 */
struct GPGGA {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	char utc_time[15];				/**< UTC time, HH:MM:SS*/
	char latitude[15];				/**< Latitude, DDMM.SSS*/
	char north_south_indicator;		/**< N (North) or S (South)*/
	char longitude[15];				/**< Longitude, DDMM.SSS*/
	char east_west_indicator;		/**< E (East) or W (West)*/
	/**
	 * @brief Position fix
	 * 0 = invalid
	 * 1 = GPS fix (SPS)
	 * 2 = DGPS fix
	 * 3 = PPS fix
	 * 4 = Real Time Kinematic
	 * 5 = Float RTK
	 * 6 = estimated (dead reckoning)
	 * 7 = Manual input mode
	 * 8 = Simulation mode
	 */
	int16_t position_fix;
	int16_t satellites_used;		/**< Number of satellites being tracked*/
	float hdop;						/**< Horizontal dilution of position*/
	float msl_altitude;				/**< Altitude above mean sea level*/
	char msl_unit[5];				/**< Unit (usually in meters)*/
	float geoid_separation;			/**< Height of geoid (mean sea level) above WGS84 ellipsoid*/
	char geoid_unit[5];				/**< Unit (usually in meters)*/
	float age_of_diff_corr;			/**< time in seconds since last DGPS update*/
	int16_t station_id;				/**< DGPS station ID number*/
};

/**
 * GSA - GPS DOP and active satellites.
 * Example: $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
 * This sentence provides details on the nature of the fix. It includes the numbers of the satellites being used in the current solution and the DOP. DOP (dilution of precision) is an indication of the effect of satellite geometry on the accuracy of the fix. It is a unitless number where smaller is better. For 3D fixes using 4 satellites a 1.0 would be considered to be a perfect number, however for overdetermined solutions it is possible to see numbers below 1.0.
 * There are differences in the way the PRN's are presented which can effect the ability of some programs to display this data. For example, in the example shown below there are 5 satellites in the solution and the null fields are scattered indicating that the almanac would show satellites in the null positions that are not being used as part of this solution. Other receivers might output all of the satellites used at the beginning of the sentence with the null field all stacked up at the end. This difference accounts for some satellite display programs not always being able to display the satellites being tracked. Some units may show all satellites that have ephemeris data without regard to their use as part of the solution but this is non-standard.
 */
struct GPGSA {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	char mode1;						/**< A=automatic, allowed to switch 2D/3D, M=manual, forced to operate in 2D/3D*/
	/**
	 * @brief 3D fix - values include
	 * 1 = no fix
     * 2 = 2D fix
     * 3 = 3D fix
	 */
	int16_t mode2;
	int16_t sat_channel_1;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_2;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_3;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_4;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_5;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_6;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_7;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_8;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_9;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_10;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_11;			/**< PRNs of satellite used for fix*/
	int16_t sat_channel_12;			/**< PRNs of satellite used for fix*/
	float pdop;						/**< PDOP (dilution of precision), 1.0 is perfect */
	float hdop;						/**< Horizontal dilution of precision (HDOP), 1.0 is perfect*/
	float vdop;						/**< Vertical dilution of precision (VDOP), 1.0 is perfect*/
};

/**
 * GSV - Satellites in View shows data about the satellites that the unit might be able to find based on its viewing mask and almanac data.
 * Example: $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75
 * It also shows current ability to track this data. Note that one GSV sentence only can provide data for up to 4 satellites and thus there may need to be 3 sentences for the full information. It is reasonable for the GSV sentence to contain more satellites than GGA might indicate since GSV may include satellites that are not used as part of the solution. It is not a requirment that the GSV sentences all appear in sequence. To avoid overloading the data bandwidth some receivers may place the various sentences in totally different samples since each sentence identifies which one it is.
 * The field called SNR (Signal to Noise Ratio) in the NMEA standard is often referred to as signal strength. SNR is an indirect but more useful value that raw signal strength. It can range from 0 to 99 and has units of dB according to the NMEA standard, but the various manufacturers send different ranges of numbers with different starting numbers so the values themselves cannot necessarily be used to evaluate different units. The range of working values in a given gps will usually show a difference of about 25 to 35 between the lowest and highest values, however 0 is a special case and may be shown on satellites that are in view but not being tracked.
 */
struct GPGSV {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	int16_t number_of_messages;		/**< Number of sentences for full data*/
	int16_t message_idx;			/**< Sentence number*/
	int16_t sats_in_view;			/**< Number of satellites in view*/
	int16_t sat1_id;				/**< Satellite PRN number*/
	int16_t sat1_elevation;			/**< Elevation, degrees, from 01 to 90*/
	int16_t sat1_azimuth;			/**< Azimuth, degrees, from 000 to 359*/
	int16_t sat1_snr;				/**< SNR (db-Hz) - higher is better channel, from 00 to 99, null when not tracking*/
	int16_t sat2_id;				/**< Satellite PRN number*/
	int16_t sat2_elevation;			/**< Elevation, degrees, from 01 to 90*/
	int16_t sat2_azimuth;			/**< Azimuth, degrees, from 000 to 359*/
	int16_t sat2_snr;				/**< SNR (db-Hz) - higher is better channel, from 00 to 99, null when not tracking*/
	int16_t sat3_id;				/**< Satellite PRN number*/
	int16_t sat3_elevation;			/**< Elevation, degrees, from 01 to 90*/
	int16_t sat3_azimuth;			/**< Azimuth, degrees, from 000 to 359*/
	int16_t sat3_snr;				/**< SNR (db-Hz) - higher is better channel, from 00 to 99, null when not tracking*/
	int16_t sat4_id;				/**< Satellite PRN number*/
	int16_t sat4_elevation;			/**< Elevation, degrees, from 01 to 90*/
	int16_t sat4_azimuth;			/**< Azimuth, degrees, from 000 to 359*/
	int16_t sat4_snr;				/**< SNR (db-Hz) - higher is better channel, from 00 to 99, null when not tracking*/
};

/**
 * HCHDG - Compass output is used on Garmin etrex summit, vista , and 76S receivers to output the value of the internal flux-gate compass. Only the magnetic heading and magnetic variation is shown in the message.
 * Example: $HCHDG,101.1,,,7.1,W*3C
 */
struct HCHDG {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	int16_t heading;				/**< heading*/
	float deviation;				/**< deviation*/
	char dev_direction;				/**< deviation direction*/
	float variation;				/**< variation*/
	char var_direction;				/**< variation direction*/
};

/*
	$GPRMC
	Recommended Minimum Specific GNSS Data
	Sentence ID		 $GPRMC
	UTC Time			171102.081 (hhmmss.sss)
	Status			  A (A=datavalid, V=data not valid)
	Latitude			4835.7692 (ddmm.mmmm)
	N/S indicator	   N (N=north, S=south)
	Longitude		   00220.9694 (dddmm.mmmm)
	E/W Indicator	   E (E=east, W=West)
	Speed over ground   0.24 (knots)
	Course over ground  210 ((degrees) Not valid at low speed. LS20126 may derive it from magnetic heading in this case)
	Date				121011 (ddmmyy)
	Magnetic variation  0.2 (degree)
	Variation sense	 W (W=west, E=east)
	Mode				A (A=autonomous, D=DGPS, E=DR)
	Checksum			*33
*/
struct GPRMC {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	char utc_time[15];				/**< */
	char status;					/**< */
	char latitude[15];				/**< */
	char north_south_indicator;		/**< */
	char longitude[15];				/**< */
	char east_west_indicator;		/**< */
	float speed_over_ground;		/**< */
	float course_over_ground;		/**< */
	char date[10];					/**< */
	float magnetic_variation;		/**< */
	char variation_sense;			/**< */
	char mode;						/**< */
};

/*
	$GPGLL
	Geographic Position, Latitude / Longitude and time.
	$GPGLL,4448.55381,N,00038.83314,W,134006.00,A,A*7B
	Sentence ID		 $GPGLL
	Latitude			4448.55381 (ddmm.mmmmm)
	N/S indicator	   N (N=north, S=south)
	Longitude		   00048.83314 (dddmm.mmmm)
	E/W Indicator	   W (E=east, W=West)
	Fix time			134006.00 (13h40mn06.00s UTC)
	Data Active or void A (or V)
*/
struct GPGLL {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	char latitude[15];				/**< */
	char north_south_indicator;		/**< */
	char longitude[15];				/**< */
	char east_west_indicator;		/**< */
	char fix_time[15];				/**< */
	char data_active;				/**< */
};

/*
	$GPVTG
	VTG - Velocity made good.
	$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
	VTG		  Track made good and ground speed
	054.7,T	  True track made good (degrees)
	034.4,M	  Magnetic track made good
	005.5,N	  Ground speed, knots
	010.2,K	  Ground speed, Kilometers per hour
	*48		  Checksum

	An extra field might exit right before checksum:
	Mode: A=Autonomous, D=DGPS, E=DR
*/
struct GPVTG {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	float measured_heading_1;		/**< */
	char north_type_1;				/**< */
	float measured_heading_2;		/**< */
	char north_type_2;				/**< */
	float ground_speed_1;			/**< */
	char ground_speed_unit_1;		/**< */
	float ground_speed_2;			/**< */
	char ground_speed_unit_2;		/**< */

	char mode;
};

/*
	$GPTXT
	Text message
	$GPTXT,01,01,02,u-blox ag - www.u-blox.com*50
	01						  Total number of messages, 01 to 99
	01						  Sentence number, 01 to 99
	02						  Text identifier, 01 to 99
	u-blox ag - www.u-blox.com  Text message, up to 61 characters
	*50						 Checksum
*/
struct GPTXT {
	bool isValid;					/**< This tells if struct's content is issued from a valid sentence. If not, do not use struct's datas.*/
	int16_t fieldValidity;			/**< Each bit of this int16_t telle if the corresponding field is valid of not. */

	uint8_t number_of_messages;		/**< */
	uint8_t sentence_number;		/**< */
	uint8_t text_identifier;		/**< */
	char message[70];				/**< */
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
#ifdef ARDUINO
	bool dispatch(const String str);
#else
	bool dispatch(const std::string str);
#endif
	bool dispatch(const char *str);

	//types
	enum STRINGS_TYPES {
		UNKNOWN=0,
		TYPE_PLSR2451,
		TYPE_PLSR2452,
		TYPE_PLSR2457,
		TYPE_GPGGA,
		TYPE_GPGSA,
		TYPE_GPGSV,
		TYPE_HCHDG,
		TYPE_GPRMC,
		TYPE_GPGLL,
		TYPE_GPVTG,
		TYPE_GPTXT
	};
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
	bool parse_gpgll(const char *str);
	bool parse_gpvtg(const char *str);
	bool parse_gptxt(const char *str);

	PLSR2451 last_plsr2451 = {};
	PLSR2452 last_plsr2452 = {};
	PLSR2457 last_plsr2457 = {};
	GPGGA	last_gpgga = {};
	GPGSA	last_gpgsa = {};
	GPGSV	last_gpgsv = {};
	HCHDG	last_hchdg = {};
	GPRMC	last_gprmc = {};
	GPGLL	last_gpgll = {};
	GPVTG	last_gpvtg = {};
	GPTXT	last_gptxt = {};

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
