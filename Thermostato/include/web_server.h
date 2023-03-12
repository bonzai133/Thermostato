#ifndef _web_server_h_
#define _web_server_h_

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebServer {
  private:
    float m_temperature = 0.00;
    void initWebSocket(void);
    void HandleMetrics(AsyncWebServerRequest *request);
  public:
    WebServer();
    ~WebServer();

    void initServer();
    void serverCleanup();

    void setTemperature(float value) { m_temperature = value; };

};

#endif