#include "time_utils.h"

#include <Arduino.h>
#include <TZ.h>
#include "config.h"
#include "time.h"

const char day_week_fr[7][4] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
const char month_fr[12][5] = {"Jan", "Fév", "Mar", "Avr", "Mai", "Jui", "Jul", "Aou", "Sep", "Oct", "Nov", "Déc"};

void init_time() {
  configTime(MY_TZ, NTP_SERVER);

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
  String my_date;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    char day[3];

    const char *wday = day_week_fr[timeinfo.tm_wday];

    strftime(day, 3, "%e", &timeinfo);
    day[2] = 0;

    const char *month = month_fr[timeinfo.tm_mon];
    
    my_date.concat(wday);
    my_date.concat(" ");
    my_date.concat(day);
    my_date.concat(" ");
    my_date.concat(month);

  }
  return my_date;
}
