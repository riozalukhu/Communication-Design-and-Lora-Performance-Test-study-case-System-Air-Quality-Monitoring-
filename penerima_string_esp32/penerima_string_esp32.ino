#include <LoRa.h>
#include <ArduinoJson.h>

#define BAUD_RATE 9600

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial);

  Serial2.begin(BAUD_RATE);
  while (!Serial2);

  if (!LoRa.begin(920E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.setSpreadingFactor(7); // diganti dengan rentang 7, 8, 9, 10, 11, 12
  //LoRa.setSignalBandwidth(500E3); //125, 250, 500
  LoRa.setCodingRate4(8); //5, 6, 7, 8
  // Set up WiFi and MQTT
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Read the received data
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
   
    Serial.println(message);

    size_t dataSize = message.length() * sizeof(char);
    Serial.print("Data size (bytes): ");
    Serial.println(dataSize);


    // Separate data into tokens based on commas
    String tokens[4]; // Assume there are 4 tokens in the data
    int tokenIndex = 0;
    int lastIndex = 0;
    for (int i = 0; i < message.length(); i++) {
      if (message.charAt(i) == ',') {
        tokens[tokenIndex++] = message.substring(lastIndex, i);
        lastIndex = i + 1;
      }
    }
    tokens[tokenIndex] = message.substring(lastIndex);

    // Check the number of tokens
    if (tokenIndex == 3) {
      const char* id = tokens[0].c_str();
      int mq4PPM = tokens[1].toInt();
      int mq7PPM = tokens[2].toInt();
      int dustDensity = tokens[3].toInt();

      // Create a JSON object
      StaticJsonDocument<100> doc;
      doc["id"] = id;
      doc["mq4"] = mq4PPM;
      doc["mq7"] = mq7PPM;
      doc["debu"] = dustDensity;
      doc["rssi"] = LoRa.packetRssi();
      doc["snr"] = LoRa.packetSnr();

      // Convert the JSON object to a JSON string
      String json;
      serializeJson(doc, json);

      // Send the JSON string via Serial to ESP32
      Serial.println(json);
      Serial2.println(json);
    }
  }
}
