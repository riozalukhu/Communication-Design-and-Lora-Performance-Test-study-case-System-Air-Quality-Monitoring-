#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#define RX2 16
#define TX2 17


#define WIFI_SSID "Y35"
#define WIFI_PASSWORD "1sampai4"

#define MQTT_SERVER "192.168.187.19"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC "data/ta2"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial2.begin(9600);
  while (!Serial2);
  
  if (!WiFi.begin(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Failed to connect to WiFi");
    while (1);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (mqttClient.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT broker!");
    }
    else {
      Serial.print("Failed to connect to MQTT broker with state ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void loop() {
  if (Serial2.available()) {
    String message = Serial2.readString();
    Serial.print(message);
    StaticJsonDocument<256> jsonDoc;

    // split the message by line and publish each non-empty line separately
    int index = 0;
    while ((index = message.indexOf('\n')) != -1) {
      String line = message.substring(0, index);
      message = message.substring(index + 1);

      // Skip empty lines
      if (line.length() == 0) {
        continue;
      }

      DeserializationError error = deserializeJson(jsonDoc, line.c_str());
      if (error) {
        continue;
      }

      mqttClient.publish(MQTT_TOPIC, line.c_str());
    }

    // publish the last line if it's non-empty
    if (message.length() > 0) {
      mqttClient.publish(MQTT_TOPIC, message.c_str());
    }
  }

  mqttClient.loop();
}
