#ifndef _time_utils_h_
#define _time_utils_h_

#include <Arduino.h>

struct day_hour_minute {
  int day;
  int hour;
  int minute;
};

void init_time(String timezone, String ntpServer);
String getFormattedTime();
String getFormattedDate();
bool getDate(day_hour_minute *dhm);

#endif