#ifndef _settings_h_
#define _settings_h_

#include <Arduino.h>
#include "heating_periods.h"

struct persistentData {
  char tempConfort[5];
  char tempDeltaConfort[5];
  char tempEco[5];
  char tempDeltaEco[5];
  char tempHorsGel[5];
  char tempDeltaHorsGel[5];

  char heatingMode; // P, C, E, H
};

class Settings {
  private:
    struct persistentData m_persistentData;
    HeatingPeriods m_HeatingPeriods;

    String m_ipAddress;
    String m_ntpServer;
    const char *m_timezone;
    int m_contrast;

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
    String getTempConfort() { return m_persistentData.tempConfort; };
    String getTempDeltaConfort() { return m_persistentData.tempDeltaConfort; };
    String getTempEco() { return m_persistentData.tempEco; };
    String getTempDeltaEco() { return m_persistentData.tempDeltaEco; };
    String getTempHorsGel() { return m_persistentData.tempHorsGel; };
    String getTempDeltaHorsGel() { return m_persistentData.tempDeltaHorsGel; };

    String getTempSetpoint() {
      String temp;

      if (m_persistentData.heatingMode == 'P') {
        // TODO: check current prog mode
        temp = m_persistentData.tempEco;
      } else if (m_persistentData.heatingMode == 'C') {
        temp = m_persistentData.tempConfort;
      } else if (m_persistentData.heatingMode == 'E') {
        temp = m_persistentData.tempEco;
      } else if (m_persistentData.heatingMode == 'H') {
        temp = m_persistentData.tempHorsGel;
      }

      return temp;
    }

    String getTempDelta() {
      String delta;

      if (m_persistentData.heatingMode == 'P') {
        // TODO: check current prog mode
        delta = m_persistentData.tempDeltaEco;
      } else if (m_persistentData.heatingMode == 'C') {
        delta = m_persistentData.tempDeltaConfort;
      } else if (m_persistentData.heatingMode == 'E') {
        delta = m_persistentData.tempDeltaEco;
      } else if (m_persistentData.heatingMode == 'H') {
        delta = m_persistentData.tempDeltaHorsGel;
      }

      return delta;
    }

    void setHeatingMode(String value) {
      char mode = 'E';
      
      if (value.equals("prog")) { mode = 'P'; }
      else if (value.equals("confort")) { mode = 'C'; }
      else if (value.equals("eco")) { mode = 'E'; }
      else if (value.equals("horsgel")) { mode = 'H'; }

      m_persistentData.heatingMode = mode;

    };
    void setTempConfort(String value) { snprintf(m_persistentData.tempConfort, 5, value.c_str()); };
    void setTempDeltaConfort(String value) { snprintf(m_persistentData.tempDeltaConfort, 5, value.c_str()); };
    void setTempEco(String value) { snprintf(m_persistentData.tempEco, 5, value.c_str()); };
    void setTempDeltaEco(String value) { snprintf(m_persistentData.tempDeltaEco, 5, value.c_str()); };
    void setTempHorsGel(String value) { snprintf(m_persistentData.tempHorsGel, 5, value.c_str()); };
    void setTempDeltaHorsGel(String value) { snprintf(m_persistentData.tempDeltaHorsGel, 5, value.c_str()); };

    void commit() { SaveConfig(); }

    String getIpAddress() { return m_ipAddress; };
    String getNtpServer() { return m_ntpServer; };
    String getTimezone() { return m_timezone; };
    int getContrast() { return m_contrast; };

    void setIpAddress(String ipAddress) { m_ipAddress = ipAddress; };
    void setContrast(int value) {  m_contrast = value; };

};
#endif