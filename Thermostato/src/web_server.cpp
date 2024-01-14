#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>
#include <WebSerial.h>

#include "web_server.h"
#include "LittleFS.h"
#include "config.h"
#include "time_utils.h"

AsyncWebServer server(80);

WebServer::WebServer(Settings* settings, HeatingControl* heatingControl) {
  m_settings = settings;
  m_heatingControl = heatingControl;

}

WebServer::~WebServer() {

}

void WebServer::HandleMetrics(AsyncWebServerRequest *request) {
  String response = "";

  response += "# HELP thermostato_heating_mode Heating mode: Prog, Confort, Eco, Horgel.\n";
  response += "# TYPE thermostato_heating_mode gauge\n";
  response += "thermostato_heating_mode ";
  response += m_settings->getHeatingMode();
  response += "\n";

  response += "# HELP thermostato_temperature_setpoint Temperature setpoint for current mode in Celsius.\n";
  response += "# TYPE thermostato_temperature_setpoint gauge\n";
  response += "thermostato_temperature_setpoint ";
  response += m_settings->getTempSetpoint();
  response += "\n";

  response += "# HELP thermostato_temperature_delta Temperature delta for current mode.\n";
  response += "# TYPE thermostato_temperature_delta gauge\n";
  response += "thermostato_temperature_delta ";
  response += m_settings->getTempDelta();
  response += "\n";

  response += "# HELP thermostato_temperature_celsius Current temperature in Celsius.\n";
  response += "# TYPE thermostato_temperature_celsius gauge\n";
  response += "thermostato_temperature_celsius ";
  response += m_heatingControl->getTemperature(2);
  response += "\n";

  response += "# HELP thermostato_pump_status Pump status on (1) / off (0).\n";
  response += "# TYPE thermostato_pump_status gauge\n";
  response += "thermostato_pump_status ";
  response += m_heatingControl->isHeating();
  response += "\n";

  response += "# HELP thermostato_heating_time_seconds Current heating time.\n";
  response += "# TYPE thermostato_heating_time_seconds gauge\n";
  response += "thermostato_heating_time_seconds ";
  response += m_heatingControl->getHeatingTimeSeconds();
  response += "\n";

  response += "# HELP thermostato_last_heating_time_seconds Last heating time.\n";
  response += "# TYPE thermostato_last_heating_time_seconds gauge\n";
  response += "thermostato_last_heating_time_seconds ";
  response += m_heatingControl->getLastHeatingTimeSeconds();
  response += "\n";

  response += "# HELP thermostato_free_heap Free heap memory.\n";
  response += "# TYPE thermostato_free_heap gauge\n";
  response += "thermostato_free_heap ";
  response += ESP.getFreeHeap();
  response += "\n";

  response += "# HELP thermostato_reset_reason Reset reason code.\n";
  response += "# TYPE thermostato_reset_reason gauge\n";
  response += "thermostato_reset_reason ";
  response += ESP.getResetReason();

  response += "\n";
  response += "# ";
  response += ESP.getResetInfoPtr()->reason;
  response += "\n";
  response += "# ";
  response += ESP.getResetInfoPtr()->exccause;

  response += "\n";

  response += "# HELP thermostato_uptime_ms Uptime in ms.\n";
  response += "# TYPE thermostato_uptime_ms counter\n";
  response += "thermostato_uptime_ms ";
  response += millis();
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

  // For debug only
  // serializeJson(json, Serial);
  // Serial.println("");

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

  // For debug only
  // serializeJson(json, Serial);
  // Serial.println("");

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

  // For debug only
  // serializeJson(json, Serial);
  // Serial.println("");

  request->send(response);
}

void WebServer::HandleGetAdvancedConfig(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument json(1024);

  json["date"] = getFormattedDate();
  json["time"] = getFormattedTime();
  json["ipAddress"] = m_settings->getIpAddress();
  json["timezone"] = m_settings->getTimezone();
  json["ntpServer"] = m_settings->getNtpServer();

  json["lcdContrast"] = String(m_settings->getContrast());
  json["tempOffset"] = String(m_settings->getTempOffset());

  serializeJson(json, *response);
  // For debug only
  // serializeJson(json, Serial);
  // Serial.println("");
  
  request->send(response);
}

void WebServer::HandleGetTimeSlots(AsyncWebServerRequest *request) {
  // Serial.println("GET /api/timeSlots");

  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument doc(MY_CONFIG_SIZE_JSON_TIMESLOT);
  JsonArray tsArray = doc.createNestedArray("timeSlots");

  m_settings->getTimeSlots(tsArray);

  serializeJson(doc, *response);

  // For debug only
  // serializeJson(doc, Serial);
  // Serial.println("");

  request->send(response);
}

String WebServer::Processor(const String& var)
{
  if(var == "VERSION")
    return F(VERSION);
  return String();
}

void WebServer::initServer(void) {
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
  server.on("/api/advancedConfig", HTTP_GET, std::bind(&WebServer::HandleGetAdvancedConfig, this, std::placeholders::_1));
  server.on("/api/timeSlots", HTTP_GET, std::bind(&WebServer::HandleGetTimeSlots, this, std::placeholders::_1));

  // TODO: how to use WebServer::HandlePutConfig ?
  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/api/config", [this](AsyncWebServerRequest *request, JsonVariant &json) {
    // Serial.println("POST /api/config");
    // For debug only
    // serializeJson(json, Serial);
    // Serial.println("");

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
      m_settings->SaveConfig();
      request->send(200, "application/json", "{\"message\":\"OK\"}");
    } else {
      request->send(400, "application/json", "{\"message\":\"Invalid parameters\"}");
    }
  });
  server.addHandler(handler);


  AsyncCallbackJsonWebHandler* handler2 = new AsyncCallbackJsonWebHandler("/api/advancedConfig", [this](AsyncWebServerRequest *request, JsonVariant &json) {
    // Serial.println("POST /api/advancedConfig");
    // For debug only
    // serializeJson(json, Serial);
    // Serial.println("");

    JsonObject jsonObj = json.as<JsonObject>();

    // Save config
    if(jsonObj.containsKey("lcdContrast")) {
      m_settings->setContrast(jsonObj["lcdContrast"]);
      m_settings->setTempOffset(jsonObj["tempOffset"]);
      m_settings->SaveConfig();
      request->send(200, "application/json", "{\"message\":\"OK\"}");
    } else {
      request->send(400, "application/json", "{\"message\":\"Invalid parameters\"}");
    }
  });
  server.addHandler(handler2);


  AsyncCallbackJsonWebHandler* handler3 = new AsyncCallbackJsonWebHandler("/api/timeSlots", [this](AsyncWebServerRequest *request, JsonVariant &json) {
    // Serial.println("POST /api/timeSlots");
    // For debug only
    // serializeJson(json, Serial);
    // Serial.println("");

    JsonObject jsonObj = json.as<JsonObject>();

    // Save config
    if(jsonObj.containsKey("timeSlots")) {
      m_settings->setTimeSlots(jsonObj["timeSlots"]);
      m_settings->SaveTimeSlots();
      request->send(200, "application/json", "{\"message\":\"OK\"}");
    } else {
      request->send(400, "application/json", "{\"message\":\"Invalid parameters\"}");
    }
  }, MY_CONFIG_SIZE_JSON_TIMESLOT);
  server.addHandler(handler3);


  // Start ElegantOTA
  ElegantOTA.begin(&server);

  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  // Attach Message Callback
  WebSerial.msgCallback(std::bind(&WebServer::recvMsg, this, std::placeholders::_1, std::placeholders::_2));

  // Start server
  server.begin();
}

/* Message callback of WebSerial */
void WebServer::recvMsg(uint8_t *data, size_t len) {
  String d = "";
  for(uint i=0; i<len; i++){
    d += char(data[i]);
  }

  if(d == "on") {
    WebSerial.println("Set Heating: ");
    m_heatingControl->setHeating(true);
    WebSerial.println("Set Heating: On");
  }
  else if(d == "off") {
    WebSerial.println("Set Heating: ");
    m_heatingControl->setHeating(false);
    WebSerial.println("Set Heating: Off");
  } else if (d == "r") {
    WebSerial.println(ESP.getResetReason());
    WebSerial.println(ESP.getResetInfoPtr()->reason);
    WebSerial.println(ESP.getResetInfoPtr()->exccause);
  } else {
    WebSerial.print("Temp: ");
    WebSerial.println(m_heatingControl->getTemperature());
    WebSerial.print("Is heating: ");
    WebSerial.println(m_heatingControl->isHeating());
    WebSerial.println(m_heatingControl->getHeatingTimeSeconds());

  }
  WebSerial.println("Command processed !");
}