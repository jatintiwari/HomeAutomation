// Import required libraries
#include "WiFi.h"
#include "WifiUtils.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

AsyncWebServer server(80);

String RELAY_FILE_PATH = "/relay-state.json";
String CONFIG_FILE_PATH = "/config.json";

StaticJsonDocument<1024> INIT_STATE; // remove this global param
StaticJsonDocument<1024> Config;

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    String str;
    serializeJson(INIT_STATE, str);
    return str;
  } else if (var == "CONFIG") {
    String str;
    serializeJson(Config, str);
    return str;
  }
  return String();
}

void initServer() {
//  server
//  .serveStatic("/", SPIFFS, "/")
//  .setDefaultFile("index.html")
//  server.setTemplateProcessor(processor)
//  .setAuthentication("JT", "Yolo");

  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html",false, processor);
  });
 
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.css", "text/css");
  });
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
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
    const char* state = stateParam->value().c_str();

    Serial.print("Current relay state: ");
    Serial.print(relayIdParam->value());
    Serial.println(stateParam->value());

//    const size_t capacity = JSON_OBJECT_SIZE(7) + 400;
    StaticJsonDocument<128> doc;

//    File file = SPIFFS.open(RELAY_FILE_PATH, "r");
//    DeserializationError error = deserializeJson(doc, file);
//    file.close();
    
    int relId = relayIdParam->value().toInt();
    

    INIT_STATE[relayId] = INIT_STATE[relayId] == 0 ? 1 : 0; // toggle the state
    doc[relayId] = INIT_STATE[relayId];

    int stateVal = INIT_STATE[relayId];
    
//    doc["status"] = "ok";
//    if (error) {
//      doc["error"] = error.f_str();
//    }

//    file = SPIFFS.open(RELAY_FILE_PATH, "w");
//    serializeJson(doc, file);
//    file.close();


    serializeJson(doc, *response);
    Serial.print("Enable pin - ");
    Serial.print(relId);
    Serial.print(", pin state - ");
    Serial.println(stateVal);

    digitalWrite(relId, stateVal == 1 ? LOW : HIGH);
    request->send(response);
  });

  // Start server
  server.begin();
}

void initPersistedState() {
  Serial.print("Reading relay state - ");
  Serial.println(RELAY_FILE_PATH);
  File file = SPIFFS.open(RELAY_FILE_PATH, "r");
  deserializeJson(INIT_STATE, file);
  file.close();
}

void initPins() {
    String relayGPIOs[4] = {"26", "27", "25", "33"};
    for (int i = 1; i <= 4; i++) {
      int relId = relayGPIOs[i - 1].toInt();
      Serial.print("Enable pin - ");
      Serial.println(relId);
      INIT_STATE[relayGPIOs[i - 1]] = 0;
      pinMode(relId, OUTPUT);
      int initState = Config[relId];
      digitalWrite(relId, initState == 0 ? HIGH : LOW); // init state is 0.
    }
}

void initConfig() {
  Serial.print("Reading config...");
  File file = SPIFFS.open(CONFIG_FILE_PATH, "r");
  DeserializationError error = deserializeJson(Config, file);
  if(error) {
    Serial.print("Error reading file - ");
    Serial.println(error.f_str());
  }
  file.close();
  serializeJsonPretty(Config, Serial);
}


void setup() {

  Serial.begin(115200);
  initSPIFFS();
  
  initConfig();
  initPins();
  initWiFi();
  
  initServer();
  initPersistedState();
}

void loop() {
}
