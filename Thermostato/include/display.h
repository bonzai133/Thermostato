#ifndef _display_h_
#define _display_h_

#include <SH1106Wire.h>

class MainScreen {
  private:
    SH1106Wire *m_display;
    String m_setPointHigh;
    String m_setPointLow;
    String m_ipAddress;
    String m_temperature;

    bool m_isHeating;

  public:
    MainScreen();
    ~MainScreen();

    void progress(String info);
    void initDisplay();

    void setSetpointHigh(String value) { m_setPointHigh = value; };
    void setSetpointLow(String value) { m_setPointLow = value; };
    void setIpAddress(String value) { m_ipAddress = value; };
    void setTemperature(String value) { m_temperature = value; };

    void setIsHeating(bool value) { m_isHeating = value; }

    void drawScreen();
};


#endif