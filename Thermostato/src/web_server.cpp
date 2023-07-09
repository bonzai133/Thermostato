#include "web_server.h"
#include "LittleFS.h"
// #include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>
#include "config.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {5, 16, 10, 12};


String getOutputStates(){
  // JSONVar myArray;
  // for (int i =0; i<NUM_OUTPUTS; i++){
  //   myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
  //   myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  // }
  // String jsonString = JSON.stringify(myArray);
  // return jsonString;
  return "{}";
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

WebServer::WebServer(Settings* settings, HeatingControl* heatingControl) {
  m_settings = settings;
  m_heatingControl = heatingControl;

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
  response += m_heatingControl->getTemperature(2);
  response += "\n";

  // Send response
  request->send(200, "text/plain; charset=utf-8", response);
}

void WebServer::HandleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void WebServer::HandleAbout(AsyncWebServerRequest *request) {
      request->send(LittleFS, "/about.html", "text/html", false,
                    std::bind(&WebServer::Processor, this, std::placeholders::_1));
}

void WebServer::HandleTemperature(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument json(100);
  json["temperature"] = m_heatingControl->getTemperature();
  serializeJson(json, *response);
  request->send(response);
}

void WebServer::HandleHomeStatus(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument json(100);
  json["temperature"] = m_heatingControl->getTemperature();
  json["mode"] = m_settings->getHeatingMode();
  
  json["consigne"] = m_settings->getTempSetpoint();
  json["delta"] = m_settings->getTempDelta();

  if (m_heatingControl->isHeating() == true) {
    json["pump"] = "On";
  } else {
    json["pump"] = "Off";
  }

  serializeJson(json, *response);
  request->send(response);
}

void WebServer::HandleGetConfig(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument json(1024);

  json["mode"] = m_settings->getHeatingMode();
  json["confort"] = m_settings->getTempConfort();
  json["deltaConfort"] = m_settings->getTempDeltaConfort();
  json["eco"] = m_settings->getTempEco();
  json["deltaEco"] = m_settings->getTempDeltaEco();
  json["horsGel"] = m_settings->getTempHorsGel();
  json["deltaHorsGel"] = m_settings->getTempDeltaHorsGel();

  serializeJson(json, *response);
  request->send(response);
}

String WebServer::Processor(const String& var)
{
  if(var == "VERSION")
    return F(VERSION);
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
    request->send(LittleFS, "/home.html", "text/html", false);
  });

  server.serveStatic("/", LittleFS, "/");

  // Not found
  server.onNotFound(std::bind(&WebServer::HandleNotFound, this, std::placeholders::_1));

  // Metrics
  server.on("/metrics", HTTP_GET, std::bind(&WebServer::HandleMetrics, this, std::placeholders::_1));

  // Web Page
  server.on("/about.html", HTTP_GET, std::bind(&WebServer::HandleAbout, this, std::placeholders::_1));

  // API
  server.on("/api/temperature", HTTP_GET, std::bind(&WebServer::HandleTemperature, this, std::placeholders::_1));
  server.on("/api/homeStatus", HTTP_GET, std::bind(&WebServer::HandleHomeStatus, this, std::placeholders::_1));
  server.on("/api/config", HTTP_GET, std::bind(&WebServer::HandleGetConfig, this, std::placeholders::_1));
  
  // TODO: how to use WebServer::HandlePutConfig ?
  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/config", [this](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject jsonObj = json.as<JsonObject>();
    bool isValid = true;

    // TODO: Check method == PUT

    // Save config
    if(jsonObj.containsKey("mode")) {
      m_settings->setHeatingMode(jsonObj["mode"]);
    }
    else {
      isValid = false;
    }

    if(jsonObj.containsKey("confort") && jsonObj.containsKey("deltaConfort") ) {
      m_settings->setTempConfort(jsonObj["confort"]);
      m_settings->setTempDeltaConfort(jsonObj["deltaConfort"]);
    } else {
      isValid = false;
    }

    if(jsonObj.containsKey("eco") && jsonObj.containsKey("deltaEco") ) {
      m_settings->setTempEco(jsonObj["eco"]);
      m_settings->setTempDeltaEco(jsonObj["deltaEco"]);
    } else {
      isValid = false;
    }

    if(jsonObj.containsKey("horsGel") && jsonObj.containsKey("deltaHorsGel") ) {
      m_settings->setTempHorsGel(jsonObj["horsGel"]);
      m_settings->setTempDeltaHorsGel(jsonObj["deltaHorsGel"]);
    } else {
      isValid = false;
    }

    if (isValid) {
      m_settings->commit();
      request->send(200, "application/json", "{\"message\":\"OK\"}");
    } else {
      request->send(400, "application/json", "{\"message\":\"Invalid parameters\"}");
    }
  });
  server.addHandler(handler);

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
}
