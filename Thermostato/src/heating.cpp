#include <Arduino.h>
#include "config.h"
#include "heating.h"


HeatingControl::HeatingControl(Settings* settings, Temperature* tempSensor) {
    // Init references
    m_settings = settings;
    m_tempSensor = tempSensor;

    // Init values
    refreshExtValues();

    m_isHeating=false;

    pinMode(MY_CONFIG_RELAY_GPIO, OUTPUT);
}

HeatingControl::~HeatingControl() {


}

// Set temperature and update heating mode
void HeatingControl::refreshExtValues()
{
    setTempSetpoint(m_settings->getTempSetpoint());
    setTempDelta(m_settings->getTempDelta());

    float temp = m_tempSensor->getTemperature();
    // Check if NaN
    if (temp != temp) {
        Serial.println("Ignore Nan temperature !");
    } else {
        m_temperature = temp;

        calculateState();
    }
}

// Update heating state
void HeatingControl::calculateState(void) {
    float delta = m_temperature - m_tempSetpoint;
    // m_isHeating update
    if(m_isHeating) {
        // Stop heating if above high point
        
        if(delta >= m_tempDelta) {
            m_isHeating = false;
        }
    } else {
        // Start heating if below low point
        if(delta < -m_tempDelta) {
            m_isHeating = true;
        }
    }

    // GPIO update
    if(m_isHeating) {
        digitalWrite(MY_CONFIG_RELAY_GPIO, 1);
    } else {
        digitalWrite(MY_CONFIG_RELAY_GPIO, 0);
    }

}