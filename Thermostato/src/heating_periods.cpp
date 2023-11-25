#include <Arduino.h>
#include "heating_periods.h"

HeatingPeriods::HeatingPeriods() {
    //Init periods
    resetPeriods();
}

HeatingPeriods::~HeatingPeriods() {

}

void HeatingPeriods::resetPeriods(void) {
    //Init periods
    for (int i = 0; i < 7; i++) {
        m_dailyPeriods[i].nbUsed = 0;
    }
}

bool HeatingPeriods::addPeriod(uint dayOfWeek, uint startHour, uint startMinute, uint endHour, uint endMinute) {
    // Check input
    if (dayOfWeek >= 7) {
        return false;
    }

    uint index = m_dailyPeriods[dayOfWeek].nbUsed;
    // Return false if we can't add a new period
    if (index >= MY_CONFIG_MAX_NB_TIMESLOTS) {
        return false;
    }

    uint start = startHour * 60 + startMinute;
    uint end = endHour * 60 + endMinute;
    if (startHour < 0 || startMinute < 0 || endHour < 0 || endMinute < 0 ||
            startHour > 23 || startMinute > 59 || endHour > 23 || endMinute > 59 ||
            start >= end) {
        return false;
    }

    // Add the period
    m_dailyPeriods[dayOfWeek].nbUsed = index + 1;
    m_dailyPeriods[dayOfWeek].periods[index] = {startHour, startMinute, endHour, endMinute};
    
    return true;
}

// Return true if inside an existing period
bool HeatingPeriods::checkPeriod(uint dayOfWeek, uint hour, uint minute) {
    // Serial.printf("checkPeriod %d %d:%d", dayOfWeek, hour, minute);
    // Serial.println("");

    if (dayOfWeek >= 0 && dayOfWeek < 7 && hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59) {
        uint nbUse = m_dailyPeriods[dayOfWeek].nbUsed;
        for(uint i = 0; i < nbUse; i++) {
            HeatingPeriod period = m_dailyPeriods[dayOfWeek].periods[i];

            uint t = hour * 60 + minute;
            uint start = period.startHour * 60 + period.startMinute;
            uint end = period.endHour * 60 + period.endMinute;

            if (start <= t && t <= end) {
                return true;
            }
        }
        // Serial.println("No matching period");
        return false;

    } else {
        Serial.println("Invalid parameters");
        return false;
    }
}