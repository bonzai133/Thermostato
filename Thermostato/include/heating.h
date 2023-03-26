#ifndef _heating_h_
#define _heating_h_

class HeatingControl {
  private:
    float m_tempSetpoint;
    float m_tempDelta;
    float m_temperature;
    bool m_isHeating;

    void update(void);

  public:
    HeatingControl();
    ~HeatingControl();

    void setTempSetpoint(String value) { m_tempSetpoint = value.toFloat(); };
    void setTempDelta(String value) { m_tempDelta = value.toFloat(); };
    bool isHeating() { return m_isHeating; };

    void setTemperature(float value);
};

#endif