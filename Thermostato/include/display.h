#ifndef _display_h_
#define _display_h_

#include <SH1106Wire.h>
#include "settings.h"
#include "heating.h"


class MainScreen {
  private:
    SH1106Wire *m_display;

    Settings* m_settings;
    HeatingControl* m_heatingControl;
    
    // Cache previous values to prevent unnecessary updates
    String m_prevTemp;
    String m_prevSetpoint;
    String m_prevTime;
    String m_prevDate;
    bool m_prevHeating;
    uint8_t m_prevContrast;

  public:
    MainScreen();
    ~MainScreen();

    void progress(String info);
    void initDisplay();

    void setSettings(Settings *settings) { m_settings = settings; }
    void setHeatingControl(HeatingControl *heatingControl) { m_heatingControl = heatingControl; }

    void drawScreen();
    
    // Clear the display completely
    void clear();
};


#endif