#ifndef _temperature_h_
#define _temperature_h_

#include <Adafruit_MCP9808.h>

class Temperature {
  private:
    Adafruit_MCP9808 *m_tempsensor;

  public:
    Temperature();
    ~Temperature();
    
    int initSensor(int address, int resolution);
    float getTemperature();

};

#endif