#include <Arduino.h>
#include "heating_periods.h"

HeatingPeriods::HeatingPeriods() {
    //Init periods
    for (int i = 0; i < 7; i++) {
        m_dailyPeriods[i].nbUsed = 0;
    }
}

HeatingPeriods::~HeatingPeriods() {

}

bool HeatingPeriods::addPeriod(int dayOfWeek, int startHour, int startMinute, int endHour, int endMinute) {
    // Check input
    if (dayOfWeek >= 0 && dayOfWeek < 7) {
        return false;
    }

    int index = m_dailyPeriods[dayOfWeek].nbUsed + 1;
    if (index > sizeof(m_dailyPeriods[dayOfWeek].periods)) {
        return false;
    }

    int start = startHour * 60 + startMinute;
    int end = endHour * 60 + endMinute;
    if (startHour < 0 || startMinute < 0 || endHour < 0 || endMinute < 0 ||
            startHour > 23 || startMinute > 59 || endHour > 23 || endMinute > 59 ||
            start >= end) {
        return false;
    }

    // Add the period
    m_dailyPeriods[dayOfWeek].nbUsed = index;
    m_dailyPeriods[dayOfWeek].periods[index] = {startHour, startMinute, endHour, endMinute};
    
    return true;
}

// Return true if inside an existing period
bool HeatingPeriods::checkPeriod(int dayOfWeek, int hour, int minute) {
    if (dayOfWeek >= 0 && dayOfWeek < 7 && hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59) {
        int nbUse = m_dailyPeriods[dayOfWeek].nbUsed;
        for(int i = 0; i < nbUse; i++) {
            HeatingPeriod period = m_dailyPeriods[dayOfWeek].periods[i];

            int t = hour * 60 + minute;
            int start = period.startHour * 60 + period.startMinute;
            int end = period.endHour * 60 + period.endMinute;

            if (start <= t && t <= end) {
                return true;
            }
        }
        return false;

    } else {
        return false;
    }

}