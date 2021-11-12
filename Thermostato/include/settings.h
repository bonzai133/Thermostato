#ifndef _settings_h_
#define _settings_h_

#include <Arduino.h>

struct persistentData {
  char tempHigh[5];
  char tempLow[5]; 
  int  sensorAddr;
  int  sensorResolution;
};

class Settings {
  private:
    struct persistentData m_persistentData;

  public:
    Settings();
    ~Settings();

    void LoadConfig();
    void SaveConfig();
    String getTempHigh() {
        return m_persistentData.tempHigh;
    };
    String getTempLow() {
        return m_persistentData.tempLow;
    };
    int getSensorAddr() {
      return m_persistentData.sensorAddr;
    }
    int getSensorResolution() {
      return m_persistentData.sensorResolution;
    }
};
#endif