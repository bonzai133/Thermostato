#ifndef _config_h_
#define _config_h_

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_TZ_OFFSET 7200
#define NTP_REFRESH 60000

#endif