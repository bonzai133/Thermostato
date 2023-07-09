#ifndef _web_server_h_
#define _web_server_h_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "settings.h"
#include "AsyncJson.h"
#include "settings.h"
#include "heating.h"


class WebServer {
  private:
    Settings *m_settings;
    HeatingControl *m_heatingControl;

    void initWebSocket(void);
    void HandleMetrics(AsyncWebServerRequest *request);
    void HandleNotFound(AsyncWebServerRequest *request);
    void HandleAbout(AsyncWebServerRequest *request);
    void HandleTemperature(AsyncWebServerRequest *request);
    void HandleHomeStatus(AsyncWebServerRequest *request);
    void HandleGetConfig(AsyncWebServerRequest *request);
    void HandleGetAdvancedConfig(AsyncWebServerRequest *request);

    String Processor(const String& var);

  public:
    WebServer(Settings* settings, HeatingControl* heatingControl);
    ~WebServer();

    void initServer();
    void serverCleanup();

};

#endif