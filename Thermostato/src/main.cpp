/*
  Laurent PETIT

  Connected thermostat based on ESP8266
  https://github.com/bonzai133/Thermostato

*/

#include <Arduino.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#include "time_utils.h"
#include "config.h"
#include "display.h"
#include "web.h"

// secrets.h must contains the secrets that must not be shared on git
// const char* ssid = "";
// const char* password = "";
#include "secrets.h"

#define MY_SDA (0)
#define MY_SCL (2)

MainScreen mainScreen;

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
  display_IP(WiFi.localIP().toString());
}

void setup(void) {
  // Init Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setup start");

  // Init I2C
  Wire.begin(MY_SDA, MY_SCL);

  // Init Oled
  init_display();
  display_hello1();

  // Init Wifi
  init_wifi();

  // Init NTP Client
  init_time();

  // Init web server
  init_server();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Websocket cleanup
  server_cleanup();

  // Draw main screen
  mainScreen.drawScreen();

  delay(15000);
}