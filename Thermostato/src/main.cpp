/*
  Laurent PETIT

  Connected thermostat based on ESP8266
  https://github.com/bonzai133/Thermostato

*/

#include <Arduino.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"

#include "settings.h"
#include "temperature.h"
#include "time_utils.h"
#include "config.h"
#include "display.h"
#include "web_server.h"

#include "heating.h"

// secrets.h must contains the secrets that must not be shared on git
// const char* ssid = "";
// const char* password = "";
#include "secrets.h"

#define MY_SDA (0)
#define MY_SCL (2)

MainScreen* mainScreen;
WebServer* webServer;
Settings* gp_settings;
Temperature* temperature;
HeatingControl* heatingControl;

void initLittleFS(void) {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void init_wifi(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  mainScreen->setIpAddress(WiFi.localIP().toString());
}

void setup(void) {
  // Init Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setup start");

  // Init I2C
  Wire.begin(MY_SDA, MY_SCL);

  // Init Oled
  mainScreen = new MainScreen();
  mainScreen->initDisplay();

  // Settings
  mainScreen->progress("Settings");
  initLittleFS();
  gp_settings = new Settings();
  gp_settings->LoadConfig();

  // Init temp
  mainScreen->progress("Temp sensor");
  temperature = new Temperature();
  temperature->initSensor(MY_CONFIG_TEMP_SENSOR_ADDR, MY_CONFIG_TEMP_SENSOR_RESOLUTION);

  // Init heating control
  heatingControl = new HeatingControl();
  heatingControl->setTempSetpoint(gp_settings->getTempSetpoint());
  heatingControl->setTempDelta(gp_settings->getTempDelta());

  // Init Wifi
  mainScreen->progress("Wifi");
  init_wifi();

  // Init NTP Client
  mainScreen->progress("Time");
  init_time();

  // Init web server
  mainScreen->progress("Web Server");
  webServer = new WebServer();
  webServer->setSettings(gp_settings);

  webServer->initServer();
  Serial.println("HTTP server started");
}

void loop(void) {
  float temp;

  // Websocket cleanup
  webServer->serverCleanup();

  // Update paramters
  // TODO: refactor to share settings in other classes
  mainScreen->setTempSetpoint(gp_settings->getTempSetpoint());
  mainScreen->setTempDelta(gp_settings->getTempDelta());

  heatingControl->setTempSetpoint(gp_settings->getTempSetpoint());
  heatingControl->setTempDelta(gp_settings->getTempDelta());

  temp = temperature->getTemperature();
  mainScreen->setTemperature(String(temp, 1));
  heatingControl->setTemperature(temp);
  webServer->setTemperature(temp);

  // Update heat mode icon
  mainScreen->setIsHeating(heatingControl->isHeating());
  webServer->setIsHeating(heatingControl->isHeating());

  // Draw main screen
  mainScreen->drawScreen();

  delay(15000);
}