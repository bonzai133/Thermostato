#include "temperature.h"
#include "settings.h"

Temperature::Temperature(Settings* settings) {
  m_tempsensor = new Adafruit_MCP9808();
  m_settings = settings;
}

Temperature::~Temperature() {
  delete(m_tempsensor);
}

int Temperature::initSensor(int address, int resolution) {
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example, also can be left in blank for default address use
  // Also there is a table with all addres possible for this sensor, you can connect multiple sensors
  // to the same i2c bus, just configure each sensor with a different address and define multiple objects for that
  //  A2 A1 A0 address
  //  0  0  0   0x18  this is the default address
  //  0  0  1   0x19
  //  0  1  0   0x1A
  //  0  1  1   0x1B
  //  1  0  0   0x1C
  //  1  0  1   0x1D
  //  1  1  0   0x1E
  //  1  1  1   0x1F
  if (!m_tempsensor->begin(address)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    return -1;
  }
    
  Serial.println("Found MCP9808!");

  m_tempsensor->setResolution(resolution); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  return 0;    
}

float Temperature::getTemperature() {
  // wake up MCP9808 - power consumption ~200 mikro Ampere
  m_tempsensor->wake();   // wake up, ready to read!

  // Read and print out the temperature
  float c = m_tempsensor->readTempC();

  m_tempsensor->shutdown_wake(true); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling

  Serial.printf("Read temp: %f", c);
  Serial.println("");

  // Adjust temperature with offset
  c = c + m_settings->getTempOffset();

  Serial.printf("Adjusted temp: %f", c);
  Serial.println("");
  
  return c;
}
