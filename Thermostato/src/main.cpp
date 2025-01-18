/*
  Laurent PETIT

  Connected thermostat based on ESP8266
  https://github.com/bonzai133/Thermostato

*/

#include <Arduino.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ElegantOTA.h>
#include <WebSerial.h>

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
unsigned long LastMeasureTime = 0;

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
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  for(int i=0; i<120; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      break;
    } else {
      delay(500);
      Serial.print(".");
    }
  }

  // Check connection status and reset if not connected
  if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Not connected after waiting delay: will reset");
      Serial.flush();  
      ESP.restart();

      // TODO: after several restart, start without wifi (use rtc for time ?)
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  gp_settings->setIpAddress(WiFi.localIP().toString());
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

  // Init Wifi
  mainScreen->progress("Wifi");
  init_wifi();

  // Init NTP Client
  mainScreen->progress("Time");
  init_time(gp_settings->getTimezone(), gp_settings->getNtpServer());

  // Init temp
  mainScreen->progress("Temp sensor");
  temperature = new Temperature(gp_settings);
  temperature->initSensor(MY_CONFIG_TEMP_SENSOR_ADDR, MY_CONFIG_TEMP_SENSOR_RESOLUTION);

  // Init heating control
  heatingControl = new HeatingControl(gp_settings, temperature);

  // Init web server
  mainScreen->progress("Web Server");
  webServer = new WebServer(gp_settings, heatingControl);

  webServer->initServer();
  Serial.println("HTTP server started");

  // Give refrence to main screen
  mainScreen->setSettings(gp_settings);
  mainScreen->setHeatingControl(heatingControl);
}

void loop(void) {
  unsigned long CurrentTime = millis();
  
  if ((CurrentTime - LastMeasureTime) >= MY_CONFIG_LOOP_DELAY)
  {
    // Refresh external values (setpoint, current temp)
    heatingControl->refreshExtValues();
    delay(100);

    // Draw main screen
    mainScreen->drawScreen();
    delay(100);

    // Reset timer
    LastMeasureTime = CurrentTime;
  }

  ElegantOTA.loop();
  delay(100);
}