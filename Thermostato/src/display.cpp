#include "display.h"
#include "time.h"
#include "time_utils.h"
#include "icons.h"

MainScreen::MainScreen() {
  // Display base on SH1106 (OLED 128x64)
  m_display = new SH1106Wire(0x3c, -1, -1);
}

MainScreen::~MainScreen() {
  delete(m_display);
}

void MainScreen::initDisplay() {
  // Connect Wire
  m_display->connect();

  // Init display
  m_display->init();

  m_display->flipScreenVertically();

  m_display->clear();
  m_display->display();
}

void MainScreen::progress(String info) {
  m_display->clear();

  m_display->setTextAlignment(TEXT_ALIGN_LEFT);

  m_display->setFont(ArialMT_Plain_16);
  m_display->drawString(0, 10, info);

  m_display->display();
}

void MainScreen::drawScreen() {
  m_display->clear();
  m_display->setBrightness(m_settings->getContrast());

  m_display->setTextAlignment(TEXT_ALIGN_LEFT);

  m_display->setFont(ArialMT_Plain_10);

  // Date
  m_display->drawString(0, 0, getFormattedDate());

  // Setpoint temperature
  m_display->drawString(96, 42, m_settings->getTempSetpoint() + "°C");
  m_display->drawString(88, 54, "+/- " + m_settings->getTempDelta() + "°C");

  // IP address
  m_display->drawString(0, 54, m_settings->getIpAddress());

  m_display->setFont(ArialMT_Plain_16);
  // Hour
  m_display->drawString(88, 0, getFormattedTime());

  m_display->setFont(ArialMT_Plain_24);
  // current temperature
  m_display->drawString(0, 20, m_heatingControl->getTemperature(1) + "°C");

  // Draw icons
  if(m_heatingControl->isHeating()) {
    m_display->drawXbm(105, 22, 16, 16, icon_heating);
  }

  m_display->display();

}
