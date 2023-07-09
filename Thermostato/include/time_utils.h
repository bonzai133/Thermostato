#ifndef _time_utils_h_
#define _time_utils_h_

#include <Arduino.h>

void init_time(String timezone, String ntpServer);
String getFormattedTime();
String getFormattedDate();

#endif