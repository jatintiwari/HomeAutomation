#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);

String processor (const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    String str;
    serializeJson(State, str);
    return str;
  } else if (var == "CONFIG") {
    String str;
    serializeJson(Config, str);
    return str;
  }
  return String();
}

void initServer() {
  serializeJsonPretty(State, Serial);
  //  server
  //  .serveStatic("/", SPIFFS, "/")
  //  .setDefaultFile("index.html")
  //  server.setTemplateProcessor(processor)
  //  .setAuthentication("JT", "Yolo");


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.css", "text/css");
  });
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.js", "application/javascript");
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    const size_t capacity = JSON_OBJECT_SIZE(7) + 400;
    File file = SPIFFS.open(RELAY_FILE_PATH, "r");
    StaticJsonDocument<capacity> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    doc["status"] = "ok";
    if (error) {
      doc["error"] = error.f_str();
    }
    serializeJson(doc, *response);
    request->send(response);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {

    AsyncResponseStream *response = request->beginResponseStream("application/json");

    AsyncWebParameter* relayIdParam = request->getParam("relayId");
    AsyncWebParameter* stateParam = request->getParam("state");

    const char* relayId = relayIdParam->value().c_str();
    //    const char* state = stateParam->value().c_str();

    Serial.print("Current relay state: ");
    Serial.print(relayIdParam->value());
    Serial.println(stateParam->value());

    //    const size_t capacity = JSON_OBJECT_SIZE(7) + 400;
    StaticJsonDocument<128> doc;

    //    File file = SPIFFS.open(RELAY_FILE_PATH, "r");
    //    DeserializationError error = deserializeJson(doc, file);
    //    file.close();

    int relId = relayIdParam->value().toInt();
    State[relayId] = State[relayId] == 0 ? 1 : 0; // toggle the state
    doc[relayId] = State[relayId];
    //    int stateVal = state[relayId];

    //    doc["status"] = "ok";
    //    if (error) {
    //      doc["error"] = error.f_str();
    //    }

    //    file = SPIFFS.open(RELAY_FILE_PATH, "w");
    //    serializeJson(doc, file);
    //    file.close();


    serializeJson(doc, *response);
    Serial.print("Enable pin - ");
    serializeJsonPretty(doc, Serial);

    digitalWrite(relId, State[relayId] == 1 ? LOW : HIGH);
    request->send(response);
  });

  // Start server
  server.begin();
}
