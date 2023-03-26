#include <Arduino.h>
#include "config.h"
#include "heating.h"

HeatingControl::HeatingControl() {
    // Init values
    m_tempSetpoint=19.0;
    m_tempDelta=0.5;

    m_temperature=19.2;

    m_isHeating=false;

    pinMode(MY_CONFIG_RELAY_GPIO, OUTPUT);
}

HeatingControl::~HeatingControl() {


}

// Set temperature and update heating mode
void HeatingControl::setTemperature(float value)
{
    m_temperature = value;

    update();

}

// Update heating state
void HeatingControl::update(void) {
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