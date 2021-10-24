#ifndef _display_h_
#define _display_h_

#include <SH1106Wire.h>
#include <NTPClient.h>

void init_display();
void display_hello1();
void display_IP(const String &sIP);
//void display_time(const String &sTime);
void display_time(NTPClient &oClient);
void display_screen();

class MainScreen {
  private:
    SH1106Wire *m_display;
    String date;
    String setPointHigh;
    String setPointLow;
    String ipAddress;
    String time;
    String temperature;

  public:
    MainScreen();
    ~MainScreen();
    void setDate(String value);
    void setSetpointHigh(String value);
    void setSetpointLow(String value);
    void setIpAddress(String value);
    void setTemperature(String value);

    void drawScreen();

};

#endif