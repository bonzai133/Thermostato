#include <Arduino.h>
#include <WebSerial.h>
#include "config.h"
#include "heating.h"


HeatingControl::HeatingControl(Settings* settings, Temperature* tempSensor) {
    // Init references
    m_settings = settings;
    m_tempSensor = tempSensor;

    // Init values
    m_tempSetpoint = 0;
    m_tempDelta = 0;
    m_temperature = 0;

    m_isHeating = false;
    m_heatingStartTimeMs = 0;
    m_lastHeatingTimeMs = 0;

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
        WebSerial.println("Ignore Nan temperature !");
    } else {
        m_temperature = temp;

        calculateState();
    }
}

// Update heating state
void HeatingControl::calculateState(void) {
    boolean shouldHeat = false;
    float delta = m_temperature - m_tempSetpoint;

    // m_isHeating update
    if(m_isHeating) {
        // Stop heating if above SetPoint
        if(delta >= 0) {
            shouldHeat = false;
        }
    } else {
        // Start heating if below low point
        if(delta < -m_tempDelta) {
            shouldHeat = true;
        }
    }

    // Check heating ratio
    unsigned long heatTime = (millis() - m_heatingStartTimeMs) / 1000;
    
    if (shouldHeat) {
        // Currently heating longer than expected
        if (m_isHeating && heatTime > m_settings->getHeatTime()) {
            shouldHeat = false;
        }

        // Not heating, check if rest period exhausted
        if (!m_isHeating && m_lastHeatingTimeMs/1000 < m_settings->getRestTime()) {
            shouldHeat = false;
        }
    }

    // Update heating state
    setHeating(shouldHeat);
}

void HeatingControl::setHeating(boolean isHeating) {
    // Update heating start time if state change
    if (isHeating) {
        if(!m_isHeating) {
            WebSerial.print("Start heating: ");
            digitalWrite(MY_CONFIG_RELAY_GPIO, 1);
            m_heatingStartTimeMs = millis();
            WebSerial.println(m_heatingStartTimeMs);
        }
    } else {
        WebSerial.print("Stop heating: ");

        digitalWrite(MY_CONFIG_RELAY_GPIO, 0);
        m_lastHeatingTimeMs = millis() - m_heatingStartTimeMs;
        m_heatingStartTimeMs = 0;

        WebSerial.println(m_lastHeatingTimeMs);
    }

    // Update state
    m_isHeating = isHeating;
}

String HeatingControl::getHeatingTimeSeconds(void) {
    if (m_isHeating) {
        return String((millis() - m_heatingStartTimeMs) / 1000);
    }

    return String("0");
}

String HeatingControl::getLastHeatingTimeSeconds(void) {
    return String(m_lastHeatingTimeMs / 1000);
}
