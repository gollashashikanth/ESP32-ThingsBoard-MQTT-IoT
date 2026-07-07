#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define TOKEN "PEwyBC2y3f5tKjbUHty9"
#define THINGSBOARD_SERVER "thingsboard.cloud"

#define DHTPIN 15
#define DHTTYPE DHT22

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void InitWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");

    if (client.connect("ESP32", TOKEN, NULL)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  dht.begin();

  InitWiFi();

  client.setServer(THINGSBOARD_SERVER, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT Read Failed!");
    return;
  }

  String payload = "{";
  payload += "\"temperature\":";
  payload += temperature;
  payload += ",";
  payload += "\"humidity\":";
  payload += humidity;
  payload += "}";

  Serial.println(payload);

  client.publish("v1/devices/me/telemetry", payload.c_str());

  delay(5000);
}