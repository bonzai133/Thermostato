
#include "settings.h"
#include "config.h"
#include <TZ.h>
#include <LittleFS.h>
#include "time_utils.h"


Settings::Settings() {
    // Init members
    m_ntpServer = NTP_SERVER;
    m_timezone = MY_TZ;
    m_contrast = 128;

    // Init config on LittleFS
    if (!LittleFS.exists(MY_CONFIG_PATH)) {
        LittleFS.mkdir(MY_CONFIG_PATH);
    }

    if (!LittleFS.exists(MY_CONFIG_FILE)) {
        snprintf(m_persistentData.tempConfort, 5, MY_CONFIG_TEMP_CONFORT);
        snprintf(m_persistentData.tempEco, 5, MY_CONFIG_TEMP_ECO);
        snprintf(m_persistentData.tempHorsGel, 5, MY_CONFIG_TEMP_HORSGEL);

        snprintf(m_persistentData.tempDeltaConfort, 5, MY_CONFIG_TEMP_DELTA);
        snprintf(m_persistentData.tempDeltaEco, 5, MY_CONFIG_TEMP_DELTA);
        snprintf(m_persistentData.tempDeltaHorsGel, 5, MY_CONFIG_TEMP_DELTA);
        
        m_persistentData.heatingMode = MY_CONFIG_HEATING_MODE;

        SaveConfig();
    }
}

void Settings::LoadConfig() {
    File configFile = LittleFS.open(MY_CONFIG_FILE, "r");
    configFile.readBytes((char *)&m_persistentData, sizeof (persistentData));
    configFile.close();
}

void Settings::SaveConfig() {
    File configFile = LittleFS.open(MY_CONFIG_FILE, "w");
    configFile.write((char *)&m_persistentData, sizeof (persistentData));
    configFile.close();
}

String Settings::getTempSetpoint() {
    String temp;

    if (m_persistentData.heatingMode == 'P') {
        // Check current prog mode
        day_hour_minute dhm;
        if (getDate(&dhm) && m_HeatingPeriods.checkPeriod(dhm.day, dhm.hour, dhm.minute)) {
            temp = m_persistentData.tempConfort;
        } else {
            temp = m_persistentData.tempEco;
        }
    } else if (m_persistentData.heatingMode == 'C') {
    temp = m_persistentData.tempConfort;
    } else if (m_persistentData.heatingMode == 'E') {
    temp = m_persistentData.tempEco;
    } else if (m_persistentData.heatingMode == 'H') {
    temp = m_persistentData.tempHorsGel;
    }

    return temp;
}

String Settings::getTempDelta() {
    String delta;

    if (m_persistentData.heatingMode == 'P') {
         // Check current prog mode
        day_hour_minute dhm;
        if (getDate(&dhm) && m_HeatingPeriods.checkPeriod(dhm.day, dhm.hour, dhm.minute)) {
            delta = m_persistentData.tempDeltaConfort;
        } else {
            delta = m_persistentData.tempDeltaEco;
        }
           
    } else if (m_persistentData.heatingMode == 'C') {
    delta = m_persistentData.tempDeltaConfort;
    } else if (m_persistentData.heatingMode == 'E') {
    delta = m_persistentData.tempDeltaEco;
    } else if (m_persistentData.heatingMode == 'H') {
    delta = m_persistentData.tempDeltaHorsGel;
    }

    return delta;
}