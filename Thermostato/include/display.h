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

#endif