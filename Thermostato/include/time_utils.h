#ifndef _time_utils_h_
#define _time_utils_h_

#include <Arduino.h>

void init_time();
bool getLocalTime(struct tm * info);
String getFormattedTime();
String getFormattedDate();

#endif