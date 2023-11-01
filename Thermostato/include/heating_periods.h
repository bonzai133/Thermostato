#ifndef _heating_periods_h_
#define _heating_periods_h_

#include "config.h"

// Represents a single heating period
struct HeatingPeriod {
    uint startHour;
    uint startMinute;
    uint endHour;
    uint endMinute;
};

struct DailyPeriod {
  uint nbUsed;
  HeatingPeriod periods[MY_CONFIG_MAX_NB_TIMESLOTS];
};

class HeatingPeriods {
  public:
    HeatingPeriods();
    ~HeatingPeriods();

    DailyPeriod m_dailyPeriods[7];

    // return true if inside an existing period
    bool checkPeriod(uint dayOfWeek, uint hour, uint minute);

    bool addPeriod(uint dayOfWeek, uint hour, uint minute, uint endHour, uint endMinute);

};

#endif