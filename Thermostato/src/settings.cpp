
#include "settings.h"
#include "config.h"
#include <LittleFS.h>

Settings::Settings() {
    if (!LittleFS.exists(MY_CONFIG_PATH)) {
        LittleFS.mkdir(MY_CONFIG_PATH);
    }

    if (!LittleFS.exists(MY_CONFIG_FILE)) {
        snprintf(m_persistentData.tempHigh, 5, MY_CONFIG_TEMP_HIGH);
        snprintf(m_persistentData.tempLow, 5, MY_CONFIG_TEMP_LOW);
        m_persistentData.sensorAddr = MY_CONFIG_TEMP_SENSOR_ADDR;
        m_persistentData.sensorResolution = MY_CONFIG_TEMP_SENSOR_RESOLUTION;

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