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
#include <Ticker.h>

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

// Loop watchdog (lwdt) - simplified version based on sigmdel.ca example
#define LWD_TIMEOUT       12000  // Restart if loop watchdog timer reaches this time out value (12 seconds)
#define LWD_MARKER        0xABBA0000
#define LOOP_START        0
#define REFRESH_VALUES    1
#define DRAW_SCREEN       2
#define OTA_LOOP          3

static const char *moduleNames[] = {
  "start of loop()",           
  "refresh external values",                
  "draw screen", 
  "OTA loop"  
};

volatile unsigned long    lwdTime = 0;
volatile unsigned long    lwdTimeout = LWD_TIMEOUT;
volatile unsigned long    lwdWhere = LWD_MARKER | LOOP_START;
volatile boolean          lwdIncomplete = false;

Ticker lwdTicker;

void lwdRestart(void) {
  int index = (lwdWhere & 0xFFFF);
  if (lwdIncomplete) 
    Serial.printf("\n\nLoop watch dog: loop() not completed. Last seen routine: %s\n", moduleNames[index]);
  else 
    Serial.printf("\n\nLoop watch dog timed out in routine %s\n", moduleNames[index]);
  Serial.flush();  
  ESP.restart();
}

void IRAM_ATTR lwdtcb(void) {
  if (lwdTimeout - lwdTime != LWD_TIMEOUT) // time variables out of sync
    lwdWhere = 0xABBA0000 | 4; // special marker for overwritten
  else if ((lwdWhere & 0xFFFF0000) != LWD_MARKER) 
    lwdWhere = 0xABBA0000 | 4; // where variable mangled    
  else if (millis() - lwdTime < LWD_TIMEOUT)
    return;
  lwdRestart(); // lwd timed out or lwd overwritten
}

void lwdtStamp(unsigned long where) {
  lwdWhere = LWD_MARKER | (where & 0xFFFF);
}
   
void lwdtFeed(void) {
  lwdTime = millis();
  lwdTimeout = lwdTime + LWD_TIMEOUT;
  lwdIncomplete = (lwdWhere != (LWD_MARKER | LOOP_START));
  if (lwdIncomplete) { lwdRestart(); }
}

void initLittleFS(void) {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void init_wifi(void) {
  WiFi.mode(WIFI_STA);
  mainScreen->progress("Wifi: Mode set");

  WiFi.begin(ssid, password);
  mainScreen->progress("Wifi: Connecting");
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
      // Display attempt counter every 10 attempts
      if (i % 10 == 0) {
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "Wifi: Attempt %d/120", i);
        mainScreen->progress(buffer);
      }
      delay(500);
      Serial.print(".");
    }
  }

  // Check connection status and reset if not connected
  if (WiFi.status() != WL_CONNECTED) {
      mainScreen->progress("Wifi: Failed!");
      Serial.print("Not connected after waiting delay: will reset");
      Serial.flush();
      delay(2000);
      ESP.restart();

      // TODO: after several restart, start without wifi (use rtc for time ?)
  }

  mainScreen->progress("Wifi: Connected");

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

  // Setup loop watch dog
  lwdTicker.attach_ms(LWD_TIMEOUT / 2, lwdtcb);
  lwdTime = millis();

  // Clear initialization messages before entering main loop
  mainScreen->clear();
}

void loop(void) {
  lwdtFeed(); // should be first instruction in loop()
  
  unsigned long CurrentTime = millis();
  
  // Check if we need to refresh temperature and settings
  if ((CurrentTime - LastMeasureTime) >= MY_CONFIG_LOOP_DELAY)
  {
    lwdtStamp(REFRESH_VALUES);
    // Refresh external values (setpoint, current temp)
    heatingControl->refreshExtValues();
    LastMeasureTime = CurrentTime;
  }

  lwdtStamp(DRAW_SCREEN);
  // Always draw screen to handle colon blinking (drawScreen will internally
  // check if anything needs to be updated)
  mainScreen->drawScreen();

  lwdtStamp(OTA_LOOP);
  // Handle OTA updates
  ElegantOTA.loop();

  lwdtStamp(LOOP_START);  // should be last instruction in loop()
}