
#include "settings.h"
#include "config.h"
#include <LittleFS.h>

Settings::Settings() {
    if (!LittleFS.exists(MY_CONFIG_PATH)) {
        LittleFS.mkdir(MY_CONFIG_PATH);
    }

    if (!LittleFS.exists(MY_CONFIG_FILE)) {
        snprintf(m_persistentData.tempSetpoint, 5, MY_CONFIG_TEMP_SETPOINT);
        snprintf(m_persistentData.tempDelta, 5, MY_CONFIG_TEMP_DELTA);
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
