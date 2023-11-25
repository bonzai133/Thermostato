#ifndef _heating_h_
#define _heating_h_
#include "temperature.h"
#include "settings.h"

class HeatingControl {
  private:
    Settings* m_settings;
    Temperature* m_tempSensor;

    float m_tempSetpoint;
    float m_tempDelta;
    float m_temperature;
    bool m_isHeating;

    void setTempSetpoint(String value) { m_tempSetpoint = value.toFloat(); };
    void setTempDelta(String value) { m_tempDelta = value.toFloat(); };

    void calculateState(void);

  public:
    HeatingControl(Settings* settings, Temperature* tempSensor);
    ~HeatingControl();

    bool isHeating() { return m_isHeating; };
    float getTemperature() { return m_temperature; };
    String getTemperature(int precision) { return String(m_temperature, precision); };

    void refreshExtValues();
};

#endif