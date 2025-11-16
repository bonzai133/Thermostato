#include "display.h"
#include "time.h"
#include "time_utils.h"
#include "icons.h"

MainScreen::MainScreen() {
  // Display base on SH1106 (OLED 128x64)
  m_display = new SH1106Wire(0x3c, -1, -1);
  
  // Initialize cache values
  m_prevTemp = "";
  m_prevSetpoint = "";
  m_prevTime = "";
  m_prevDate = "";
  m_prevHeating = false;
  m_prevContrast = 255; // Invalid value to force first update
  m_showColon = true;
  m_lastColonToggle = 0;
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

void MainScreen::clear() {
  m_display->clear();
  m_display->display();
}

void MainScreen::drawScreen() {
  // Toggle colon every second
  unsigned long currentMillis = millis();
  if (currentMillis - m_lastColonToggle >= 1000) {
    m_showColon = !m_showColon;
    m_lastColonToggle = currentMillis;
  }
  else {
    // No need to update other parameters
    // Will be done next second
    return;
  }

  uint8_t newContrast = m_settings->getContrast();
  String newTemp = m_heatingControl->getTemperature(1);
  String newSetpoint = m_settings->getTempSetpoint();

  String newTime = getFormattedTime(m_showColon);
  String newDate = getFormattedDate();
  bool newHeating = m_heatingControl->isHeating();

  // Only update display brightness if it changed
  if (m_prevContrast != newContrast) {
    m_display->setBrightness(newContrast);
    m_prevContrast = newContrast;
  }

  m_display->setTextAlignment(TEXT_ALIGN_LEFT);

  // Update date if changed
  if (newDate != m_prevDate) {
    m_display->setFont(ArialMT_Plain_10);
    // Clear the date area only
    m_display->setColor(BLACK);
    m_display->fillRect(0, 0, 80, 10);
    m_display->setColor(WHITE);
    m_display->drawString(0, 0, newDate);
    m_prevDate = newDate;
  }

  // Update time if changed
  if (newTime != m_prevTime) {
    m_display->setFont(ArialMT_Plain_16);
    // Clear the time area only
    m_display->setColor(BLACK);
    m_display->fillRect(88, 0, 40, 16);
    m_display->setColor(WHITE);
    m_display->drawString(88, 0, newTime);
    m_prevTime = newTime;
  }

  // Update temperature if changed
  if (newTemp != m_prevTemp) {
    m_display->setFont(ArialMT_Plain_24);
    // Clear the temperature area only
    m_display->setColor(BLACK);
    m_display->fillRect(0, 20, 95, 24);
    m_display->setColor(WHITE);
    m_display->drawString(0, 20, newTemp + "°C");
    m_prevTemp = newTemp;
  }

  // Update setpoint if changed
  if (newSetpoint != m_prevSetpoint) {
    m_display->setFont(ArialMT_Plain_10);
    // Clear the setpoint area only
    m_display->setColor(BLACK);
    m_display->fillRect(96, 54, 32, 10);
    m_display->setColor(WHITE);
    m_display->drawString(96, 54, newSetpoint + "°C");
    m_prevSetpoint = newSetpoint;
  }

  // Update heating icon if changed
  if (newHeating != m_prevHeating) {
    m_display->setColor(BLACK);
    m_display->fillRect(111, 22, 16, 16);
    if (newHeating) {
      m_display->setColor(WHITE);
      m_display->drawXbm(111, 22, 16, 16, icon_heating);
    }
    m_prevHeating = newHeating;
  }

  // IP address is static, only draw it if it's not already there
  static bool ipDrawn = false;
  if (!ipDrawn) {
    m_display->setFont(ArialMT_Plain_10);
    m_display->setColor(WHITE);
    m_display->drawString(0, 54, m_settings->getIpAddress());
    ipDrawn = true;
  }

  // Call display() to refresh screen with changes (at least colon have changed)
  m_display->display();

}
