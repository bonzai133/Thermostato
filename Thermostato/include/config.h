#ifndef _config_h_
#define _config_h_

#define VERSION "0.1"

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
#define NTP_SERVER "europe.pool.ntp.org"
#define MY_TZ TZ_Europe_Paris

#define MY_CONFIG_PATH "/config"
#define MY_CONFIG_FILE "/config/thermostato.cfg"

#define MY_CONFIG_TEMP_CONFORT "19.0"
#define MY_CONFIG_TEMP_ECO "16.0"
#define MY_CONFIG_TEMP_HORSGEL "6.0"

#define MY_CONFIG_TEMP_DELTA "0.5"
#define MY_CONFIG_HEATING_MODE 'E'

#define MY_CONFIG_TEMP_SENSOR_ADDR 0x18
#define MY_CONFIG_TEMP_SENSOR_RESOLUTION 2

#define MY_CONFIG_RELAY_GPIO 5

#endif