#include "WiFi.h"
//#include "WebServer.h"


// Set your Static IP address
IPAddress local_IP(192, 168, 1, LOCAL_PORT);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

bool accessPointMode = false;

void createAccessPoint() {
//	WebServer server(80);
//	// Connect to Wi-Fi network with SSID and password
//	Serial.println("Setting AP (Access Point)…");
//	WiFi.softAP(ssid, password);
//	WiFi.softAPConfig(local_IP, gateway, subnet);
//	delay(100);
//
//	IPAddress IP = WiFi.softAPIP();
//	Serial.print("AP IP address: ");
//	Serial.println(IP);
}


void connectToWifi() {
	String hostname = "kramer";
	WiFi.mode(WIFI_STA);
	WiFi.setHostname(hostname.c_str());
	// Configures static IP address
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
	  Serial.println("STA Failed to configure");
	}
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
	delay(1000);
	Serial.println("Connecting to WiFi..");
	}
	Serial.println(WiFi.localIP());
	Serial.print("RRSI: ");
	Serial.println(WiFi.RSSI());
}

void initWiFi() {
	accessPointMode ? createAccessPoint() : connectToWifi();
}

void reconnectWifi() {
  Serial.println('reconnecting from wifi utils...');
}
