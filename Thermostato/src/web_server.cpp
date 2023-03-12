#include "web_server.h"
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {5, 16, 10, 12};


String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

WebServer::WebServer() {

}

WebServer::~WebServer() {

}

void WebServer::serverCleanup(void) {
  ws.cleanupClients();
}

void WebServer::initWebSocket(void) {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void WebServer::HandleMetrics(AsyncWebServerRequest *request) {
  String response = "";

  response += "# HELP thermostato_temperature_celsius Current temperature in celsius.\n";
  response += "# TYPE thermostato_temperature_celsius gauge\n";
  response += "thermostato_temperature_celsius ";
  response += String(m_temperature, 2);
  response += "\n";

  // Send response
  request->send(200, "text/plain; charset=utf-8", response);
}

void WebServer::HandleAbout(AsyncWebServerRequest *request) {
      request->send(LittleFS, "/about.html", "text/html", false,
                    std::bind(&WebServer::Processor, this, std::placeholders::_1));
}

String WebServer::Processor(const String& var)
{
  if(var == "VERSION")
    return F("0.1");
  return String();
}

void WebServer::initServer(void) {
  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  }

  // initLittleFS() must be done before
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  server.serveStatic("/", LittleFS, "/");

  // Metrics
  server.on("/metrics", HTTP_GET, std::bind(&WebServer::HandleMetrics, this, std::placeholders::_1));

  // Web Page
  server.on("/about", HTTP_GET, std::bind(&WebServer::HandleAbout, this, std::placeholders::_1));

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
}



