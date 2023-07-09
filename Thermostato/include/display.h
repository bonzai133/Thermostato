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

  public:
    MainScreen();
    ~MainScreen();

    void progress(String info);
    void initDisplay();

    void setSettings(Settings *settings) { m_settings = settings; }
    void setHeatingControl(HeatingControl *heatingControl) { m_heatingControl = heatingControl; }

    void drawScreen();
};


#endif