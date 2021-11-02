#include "time_utils.h"

#include <Arduino.h>
//#include <locale.h>
#include <TZ.h>
#include "config.h"
#include "time.h"

void init_time() {
  configTime(MY_TZ, NTP_SERVER);
  //setlocale(LC_TIME, MY_LOCALE);

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    char my_time[6];
    strftime(my_time, 6, "%H:%M", &timeinfo);
    Serial.println(my_time);
  }
}

bool getLocalTime(struct tm * info)
{
    uint32_t ms = 5000;
    uint32_t start = millis();
    time_t now;
    while((millis()-start) <= ms) {
        time(&now);
        localtime_r(&now, info);
        if(info->tm_year > (2016 - 1900)){
            return true;
        }
        delay(10);
    }
    return false;
}

String getFormattedTime() {
  struct tm timeinfo;
  char my_time[6];
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    strftime(my_time, 6, "%H:%M", &timeinfo);
    Serial.println(my_time);
  }

  return my_time;
}

String getFormattedDate() {
  struct tm timeinfo;
  char my_time[11];
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    my_time[0] = 0;
  } else {
    //strftime_l(my_time, 11, "%a %e %b", &timeinfo, LC_GLOBAL_LOCALE);
    strftime(my_time, 11, "%a %e %b", &timeinfo);
  }
  my_time[10] = 0;

  return my_time;
}
