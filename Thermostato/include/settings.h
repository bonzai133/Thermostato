#ifndef _settings_h_
#define _settings_h_

#include <Arduino.h>

struct persistentData {
  char tempSetpoint[5];
  char tempDelta[5];
  char heatingMode; // P, C, E, H
};

class Settings {
  private:
    struct persistentData m_persistentData;

  public:
    Settings();
    ~Settings();

    void LoadConfig();
    void SaveConfig();
    String getHeatingMode() {
      String mode = "";
      switch(m_persistentData.heatingMode) {
        case 'P': mode = "prog"; break;
        case 'C': mode = "confort"; break;
        case 'E': mode = "eco"; break;
        case 'H': mode = "horsgel"; break;
        default: break;
      }
      
      return mode;
    }
    String getTempSetpoint() { return m_persistentData.tempSetpoint; };
    String getTempDelta() { return m_persistentData.tempDelta; };

    void setHeatingMode(String value) {
      char mode = 'E';
      
      if (value.equals("prog")) { mode = 'P'; }
      else if (value.equals("confort")) { mode = 'C'; }
      else if (value.equals("eco")) { mode = 'E'; }
      else if (value.equals("horsgel")) { mode = 'H'; }

      m_persistentData.heatingMode = mode;

    };
    void setTempSetpoint(String value) { snprintf(m_persistentData.tempSetpoint, 5, value.c_str()); };
    void setTempDelta(String value) { snprintf(m_persistentData.tempDelta, 5, value.c_str()); };
    void commit() { SaveConfig(); }

};
#endif