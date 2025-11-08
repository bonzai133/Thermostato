#ifndef _temperature_h_
#define _temperature_h_

#include <Adafruit_MCP9808.h>
#include "settings.h"

class Temperature {
  private:
    Adafruit_MCP9808 *m_tempsensor;
    Settings* m_settings;

  public:
    Temperature(Settings* settings);
    ~Temperature();
    
    int initSensor(int address, int resolution);
    float getTemperature();

};

#endif