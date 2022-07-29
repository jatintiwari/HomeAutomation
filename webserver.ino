
#include "FS.h"
#include "SPIFFS.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "Credentials.h"

StaticJsonDocument<512> Config;
StaticJsonDocument<512> State;

#include "WifiUtils.h"
#include "AsyncServer.h"


void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}


void initRelayState(JsonDocument& _state) {
  Serial.print("Reading relay state - ");
  Serial.println(RELAY_FILE_PATH);
  File file = SPIFFS.open(RELAY_FILE_PATH, "r");
  deserializeJson(_state, file);
  file.close();
}

void initPins() {
  String relayGPIOs[4] = {"26", "27", "25", "33"};
  for (int i = 1; i <= 4; i++) {
    int relId = relayGPIOs[i - 1].toInt();
    Serial.print("Enable pin - ");
    Serial.println(relId);
    State[relayGPIOs[i - 1]] = 0;
    pinMode(relId, OUTPUT);
    int initState = Config[relId];
    digitalWrite(relId, initState == 0 ? HIGH : LOW); // init state is 0.
  }
}

void initConfig(JsonDocument& _config) {
  Serial.print("Reading config...");
  File file = SPIFFS.open(CONFIG_FILE_PATH, "r");
  DeserializationError error = deserializeJson(_config, file);
  if (error) {
    Serial.print("Error reading file - ");
    Serial.println(error.f_str());
  }
  file.close();
  serializeJsonPretty(_config, Serial);
}


void setup() {

  Serial.begin(115200);
  initSPIFFS();
  initWiFi();

  initConfig(Config);
  initRelayState(State);
  
  initPins();
  initServer();

}

void loop() {
//  reconnectWifi();
}
