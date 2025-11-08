#include "time_utils.h"

#include <Arduino.h>
#include <TZ.h>
#include "time.h"

const char day_week_fr[7][4] = {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"};
const char month_fr[12][5] = {"Jan", "Fév", "Mar", "Avr", "Mai", "Jui", "Jul", "Aou", "Sep", "Oct", "Nov", "Déc"};

void init_time(String timezone, String ntpServer) {
  configTime(timezone.c_str(), ntpServer);

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    char my_time[6];
    strftime(my_time, 6, "%H:%M", &timeinfo);
    // Serial.println(my_time);
  }
}

String getFormattedTime(bool showColon) {
  struct tm timeinfo;
  char hours[3];
  char minutes[3];
  
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "--:--";
  }
  
  sprintf(hours, "%02d", timeinfo.tm_hour);
  sprintf(minutes, "%02d", timeinfo.tm_min);
  
  if (showColon) {
    return String(hours) + ":" + String(minutes);
  } else {
    return String(hours) + " " + String(minutes);
  }
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

// Get day hour minute
// day: 0 -> Lun ... 6 -> Dim
bool getDate(day_hour_minute *dhm) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return false;
  } else {
    // tm_wday = 0 for Sunday -> translate to Monday = 0
    if(timeinfo.tm_wday <= 0 || timeinfo.tm_wday > 6) {
      dhm->day = 6;
    } else {
      dhm->day = timeinfo.tm_wday - 1;
    }
    
    dhm->hour = timeinfo.tm_hour;
    dhm->minute = timeinfo.tm_min;
  }

  return true;

}