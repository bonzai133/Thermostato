#ifndef _settings_h_
#define _settings_h_

#include <Arduino.h>

struct persistentData {
  char tempSetpoint[5];
  char tempDelta[5]; 
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
    String getTempSetpoint() {
        return m_persistentData.tempSetpoint;
    };
    String getTempDelta() {
        return m_persistentData.tempDelta;
    };
    int getSensorAddr() {
      return m_persistentData.sensorAddr;
    }
    int getSensorResolution() {
      return m_persistentData.sensorResolution;
    }
};
#endif