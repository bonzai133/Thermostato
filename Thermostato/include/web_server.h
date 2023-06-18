#ifndef _web_server_h_
#define _web_server_h_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "settings.h"
#include "AsyncJson.h"

class WebServer {
  private:
    float m_temperature = 0.00;
    Settings *m_settings;
    bool m_isHeating;

    void initWebSocket(void);
    void HandleMetrics(AsyncWebServerRequest *request);
    void HandleNotFound(AsyncWebServerRequest *request);
    void HandleAbout(AsyncWebServerRequest *request);
    void HandleTemperature(AsyncWebServerRequest *request);
    void HandleHomeStatus(AsyncWebServerRequest *request);
    void HandleGetConfig(AsyncWebServerRequest *request);

    String Processor(const String& var);

  public:
    WebServer();
    ~WebServer();

    void initServer();
    void serverCleanup();

    void setSettings(Settings *settings) { m_settings = settings; };
    void setTemperature(float value) { m_temperature = value; };
    void setIsHeating(bool value) { m_isHeating = value; }

};

#endif