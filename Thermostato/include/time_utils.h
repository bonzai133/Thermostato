#ifndef _time_utils_h_
#define _time_utils_h_

#include <Arduino.h>

String getFormattedTime(bool showColon = true);
String getFormattedDate();
void init_time(String timezone, String ntpServer);

// day = 0 for Monday
struct day_hour_minute {
  int day;
  int hour;
  int minute;
};

bool getDate(day_hour_minute *dhm);

#endif