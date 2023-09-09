#include <Arduino.h>

// Represents a single heating period
struct HeatingPeriod {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
};

// Represents a schedule for a single day
class DailySchedule {
public:
    DailySchedule() {
        // Initialize with default values
        for (int i = 0; i < 7; i++) {
            periods[i] = {0, 0, 0, 0};
        }
    }

    void setPeriod(int dayOfWeek, int startHour, int startMinute, int endHour, int endMinute) {
        if (dayOfWeek >= 0 && dayOfWeek < 7) {
            periods[dayOfWeek] = {startHour, startMinute, endHour, endMinute};
            
        }
    }

    HeatingPeriod getPeriod(int dayOfWeek) const {
        if (dayOfWeek >= 0 && dayOfWeek < 7) {
            return periods[dayOfWeek];
        }
        // Default to an empty period
        return {0, 0, 0, 0};
    }

private:
    HeatingPeriod periods[7];
};

// Represents a weekly schedule
class WeeklySchedule {
public:
    void setDailySchedule(int dayOfWeek, const DailySchedule& schedule) {
        if (dayOfWeek >= 0 && dayOfWeek < 7) {
            dailySchedules[dayOfWeek] = schedule;
        }
    }

    DailySchedule getDailySchedule(int dayOfWeek) const {
        if (dayOfWeek >= 0 && dayOfWeek < 7) {
            return dailySchedules[dayOfWeek];
        }
        // Default to an empty daily schedule
        return DailySchedule();
    }

private:
    DailySchedule dailySchedules[7];
};

// Example usage:
int main() {
    WeeklySchedule weeklySchedule;

    // Define heating periods for Monday
    DailySchedule mondaySchedule;
    mondaySchedule.setPeriod(0, 8, 0, 12, 0);
    mondaySchedule.setPeriod(0, 14, 0, 18, 0);

    // Set Monday's schedule in the weekly schedule
    weeklySchedule.setDailySchedule(1, mondaySchedule);

    // Retrieve Monday's schedule from the weekly schedule
    DailySchedule retrievedSchedule = weeklySchedule.getDailySchedule(1);

    // Retrieve a specific period from Monday's schedule
    HeatingPeriod period = retrievedSchedule.getPeriod(0);

    // Print the retrieved period
    printf("Monday's first period: %02d:%02d - %02d:%02d\n", period.startHour, period.startMinute, period.endHour, period.endMinute);

    return 0;
}
