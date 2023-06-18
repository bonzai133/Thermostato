
#include "settings.h"
#include "config.h"
#include <LittleFS.h>

Settings::Settings() {
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
