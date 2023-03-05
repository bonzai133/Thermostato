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
#include "web.h"

#include "heating.h"

// secrets.h must contains the secrets that must not be shared on git
// const char* ssid = "";
// const char* password = "";
#include "secrets.h"

#define MY_SDA (0)
#define MY_SCL (2)

MainScreen* mainScreen;
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
  temperature->initSensor(0x18, 2);

  // Init heating control
  heatingControl = new HeatingControl();
  heatingControl->setSetpointHigh(gp_settings->getTempHigh());
  heatingControl->setSetpointLow(gp_settings->getTempLow());

  // Init Wifi
  mainScreen->progress("Wifi");
  init_wifi();

  // Init NTP Client
  mainScreen->progress("Time");
  init_time();

  // Init web server
  mainScreen->progress("Web Server");
  init_server();
  Serial.println("HTTP server started");
}

void loop(void) {
  float temp;
  // Websocket cleanup
  server_cleanup();

  // Draw main screen
  mainScreen->setSetpointHigh(gp_settings->getTempHigh());
  mainScreen->setSetpointLow(gp_settings->getTempLow());

  temp = temperature->getTemperature();
  mainScreen->setTemperature(String(temp, 1));
  heatingControl->setTemperature(temp);

  // Update heat mode icon
  mainScreen->setIsHeating(heatingControl->isHeating());

  mainScreen->drawScreen();

  delay(15000);
}