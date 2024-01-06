#include <Arduino.h>
#include "config.h"
#include "heating.h"


HeatingControl::HeatingControl(Settings* settings, Temperature* tempSensor) {
    // Init references
    m_settings = settings;
    m_tempSensor = tempSensor;

    // Init values
    refreshExtValues();

    m_isHeating = false;
    m_heatingStartTimeMs = 0;

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
        // Stop heating if above SetPoint
        if(delta >= 0) {
            setHeating(false);
        }
    } else {
        // Start heating if below low point
        if(delta < -m_tempDelta) {
            setHeating(true);
        }
    }

    // GPIO update
    if(m_isHeating) {
        digitalWrite(MY_CONFIG_RELAY_GPIO, 1);
    } else {
        digitalWrite(MY_CONFIG_RELAY_GPIO, 0);
    }

}

void HeatingControl::setHeating(boolean isHeating) {
    // Update heating start time if state change
    if (isHeating) {
        if(!m_isHeating) {
            m_heatingStartTimeMs = millis();
        }

    } else {
        m_heatingStartTimeMs = 0;
    }

    // Update state
    m_isHeating = isHeating;
}

String HeatingControl::getHeatingTimeSeconds(void) {
    if (m_isHeating) {
        return String((m_heatingStartTimeMs - millis() / 1000));
    }

    return "0";
    
};

