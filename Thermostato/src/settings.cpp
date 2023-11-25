
#include "settings.h"
#include "config.h"
#include <TZ.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "time_utils.h"
#include "heating_periods.h"

Settings::Settings() {
    // Init members
    m_ntpServer = NTP_SERVER;
    m_timezone = MY_TZ;
    m_contrast = 128;

    // Init config on LittleFS
    if (!LittleFS.exists(MY_CONFIG_PATH)) {
        LittleFS.mkdir(MY_CONFIG_PATH);
    }

    if (!LittleFS.exists(MY_CONFIG_FILE)) {
        snprintf(m_persistentData.tempConfort, 5, MY_CONFIG_TEMP_CONFORT);
        snprintf(m_persistentData.tempEco, 5, MY_CONFIG_TEMP_ECO);
        snprintf(m_persistentData.tempHorsGel, 5, MY_CONFIG_TEMP_HORSGEL);

        snprintf(m_persistentData.tempDeltaConfort, 5, MY_CONFIG_TEMP_DELTA);
        snprintf(m_persistentData.tempDeltaEco, 5, MY_CONFIG_TEMP_DELTA);
        snprintf(m_persistentData.tempDeltaHorsGel, 5, MY_CONFIG_TEMP_DELTA);
        
        m_persistentData.heatingMode = MY_CONFIG_HEATING_MODE;
    }
}

void Settings::LoadConfig() {
    if (LittleFS.exists(MY_CONFIG_FILE)) {
        File configFile = LittleFS.open(MY_CONFIG_FILE, "r");
        configFile.readBytes((char *)&m_persistentData, sizeof (persistentData));
        configFile.close();
    }

    if (LittleFS.exists(MY_TIMESLOTS_FILE)) {
        File timeslotsFile = LittleFS.open(MY_TIMESLOTS_FILE, "r");
        timeslotsFile.readBytes((char *)&m_HeatingPeriods.m_dailyPeriods, 7 * sizeof (DailyPeriod));
        timeslotsFile.close();
    }
}

void Settings::SaveConfig() {
    File configFile = LittleFS.open(MY_CONFIG_FILE, "w");
    configFile.write((char *)&m_persistentData, sizeof (persistentData));
    configFile.close();
}

void Settings::SaveTimeSlots() {
    File timeslotsFile = LittleFS.open(MY_TIMESLOTS_FILE, "w");
    timeslotsFile.write((char *)&m_HeatingPeriods.m_dailyPeriods, 7 * sizeof (DailyPeriod));
    timeslotsFile.close();    
}

String Settings::getTempSetpoint() {
    String temp;

    if (m_persistentData.heatingMode == 'P') {
        // Check current prog mode
        day_hour_minute dhm;
        if (getDate(&dhm) && m_HeatingPeriods.checkPeriod(dhm.day, dhm.hour, dhm.minute)) {
            temp = m_persistentData.tempConfort;
        } else {
            temp = m_persistentData.tempEco;
        }
    } else if (m_persistentData.heatingMode == 'C') {
    temp = m_persistentData.tempConfort;
    } else if (m_persistentData.heatingMode == 'E') {
    temp = m_persistentData.tempEco;
    } else if (m_persistentData.heatingMode == 'H') {
    temp = m_persistentData.tempHorsGel;
    }

    return temp;
}

String Settings::getTempDelta() {
    String delta;

    if (m_persistentData.heatingMode == 'P') {
         // Check current prog mode
        day_hour_minute dhm;
        if (getDate(&dhm) && m_HeatingPeriods.checkPeriod(dhm.day, dhm.hour, dhm.minute)) {
            delta = m_persistentData.tempDeltaConfort;
        } else {
            delta = m_persistentData.tempDeltaEco;
        }
           
    } else if (m_persistentData.heatingMode == 'C') {
    delta = m_persistentData.tempDeltaConfort;
    } else if (m_persistentData.heatingMode == 'E') {
    delta = m_persistentData.tempDeltaEco;
    } else if (m_persistentData.heatingMode == 'H') {
    delta = m_persistentData.tempDeltaHorsGel;
    }

    return delta;
}

// m_HeatingPeriods to JsonArray
void Settings::getTimeSlots(JsonArray& tsArray) {
  // Add objects to the JSON array
  for (uint i = 0; i < 7; ++i) {
    DailyPeriod dp = m_HeatingPeriods.m_dailyPeriods[i];

    JsonObject obj;
    if (dp.nbUsed > 0) {
        obj = tsArray.createNestedObject();
        switch(i) {
            case 0: obj["day"] = "Mon"; break;
            case 1: obj["day"] = "Tue"; break;
            case 2: obj["day"] = "Wed"; break;
            case 3: obj["day"] = "Thu"; break;
            case 4: obj["day"] = "Fri"; break;
            case 5: obj["day"] = "Sat"; break;
            case 6: obj["day"] = "Sun"; break;
            default: obj["day"] = ""; break;
        }
    } else {
        continue;
    }
    
    JsonArray times = obj.createNestedArray("times");
    for (uint j = 0; j < dp.nbUsed; j++) {
        char tmpStr[6];
        JsonObject time = times.createNestedObject();

        sprintf(tmpStr, "%02d:%02d", dp.periods[j].startHour, dp.periods[j].startMinute);
        time["start"] = tmpStr;

        sprintf(tmpStr, "%02d:%02d", dp.periods[j].endHour, dp.periods[j].endMinute);
        time["end"] = tmpStr;
    }
  }
}

// JsonArray to m_HeatingPeriods
void Settings::setTimeSlots(JsonArray tsArray) {
    // Reset existing periods
    m_HeatingPeriods.resetPeriods();

    // Iterate periods
    uint day = 0;
    uint hour = 0;
    uint minute = 0;
    uint endHour = 0;
    uint endMinute = 0;
    bool result = false;

    for(const auto& v : tsArray) {
        if(v["day"] == "Mon") { day = 0; }
        else if(v["day"] == "Tue") { day = 1; }
        else if(v["day"] == "Wed") { day = 2; }
        else if(v["day"] == "Thu") { day = 3; }
        else if(v["day"] == "Fri") { day = 4; }
        else if(v["day"] == "Sat") { day = 5; }
        else if(v["day"] == "Sun") { day = 6; }

        JsonArray times = v["times"].as<JsonArray>();

        for(const auto& ts : times) {
            sscanf(ts["start"].as<const char*>(), "%d:%d", &hour, &minute);
            sscanf(ts["end"].as<const char*>(), "%d:%d", &endHour, &endMinute);
    
            Serial.printf("addPeriod Day=%d; Hour=%d; Minute=%d; endHour=%d; endMinute=%d", day, hour, minute, endHour, endMinute);
            Serial.println("");

            result = m_HeatingPeriods.addPeriod(day, hour, minute, endHour, endMinute);
            if(result == false) {
                Serial.println("Invalid period");
            }
        }
    }
}