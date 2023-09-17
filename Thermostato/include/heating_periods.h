#ifndef _heating_periods_h_
#define _heating_periods_h_

// Represents a single heating period
struct HeatingPeriod {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
};

struct DailyPeriod {
  int nbUsed;
  HeatingPeriod periods[10];
};

class HeatingPeriods {
  private:
    DailyPeriod m_dailyPeriods[7];

    bool addPeriod(int dayOfWeek, int hour, int minute, int endHour, int endMinute);

  public:
    HeatingPeriods();
    ~HeatingPeriods();

    // return true if inside an existing period
    bool checkPeriod(int dayOfWeek, int hour, int minute);

};

#endif