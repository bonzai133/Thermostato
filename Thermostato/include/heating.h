#ifndef _heating_h_
#define _heating_h_

class HeatingControl {
  private:
    float m_setPointHigh;
    float m_setPointLow;
    float m_temperature;
    bool m_isHeating;

    void update(void);

  public:
    HeatingControl();
    ~HeatingControl();

    void setSetpointHigh(String value) { m_setPointHigh = value.toFloat(); };
    void setSetpointLow(String value) { m_setPointLow = value.toFloat(); };
    bool isHeating() { return m_isHeating; };

    void setTemperature(float value);
};

#endif