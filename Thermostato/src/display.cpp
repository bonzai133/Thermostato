#include "display.h"
#include "icons.h"

// Display base on SH1106 (OLED 128x64)
SH1106Wire display(0x3c, -1, -1);

void init_display() {
  // Connect Wire
  display.connect();

  // Init display
  display.init();
  
  display.flipScreenVertically();

  display.clear();
  display.display();
}

void display_hello1() {
  display.clear();
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Init Wifi");

  display.display();
}

void display_IP(const String &sIP) {
  display.clear();
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Connected");
  display.drawString(0, 30, sIP);

  display.display();
}

void display_time(const String &sTime) {
  display.clear();
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 42, sTime);

  display.display();
}


void display_time(NTPClient &oClient) {
  unsigned long rawTime = oClient.getEpochTime();
  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 42, hoursStr + ":" + minuteStr);

  display.display();
}


void display_screen() {
  display.clear();
  display.setBrightness(100);

  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.setFont(ArialMT_Plain_10);
  //display.setFont(&FreeMono9pt7b);
  // Date
  display.drawString(0, 0, "Mer 20 Oct");

  // Setpoint temperature
  display.drawString(96, 42, "19.5°C");
  display.drawString(96, 54, "16.0°C");

  // IP address
  display.drawString(0, 54, "192.168.0.12");

  display.setFont(ArialMT_Plain_16);
  // Hour
  display.drawString(88, 0, "15:39");

  display.setFont(ArialMT_Plain_24);
  // current temperature
  display.drawString(0, 20, "20.8°C");

  // Draw icons
  display.drawXbm(105, 22, 16, 16, sun_icon);

  display.display();
}
