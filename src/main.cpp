#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define RXD2 16
#define TXD2 17

// FZsnB2nUX72yyyPpduzujIjbtlYgWPpFfKy-2VVZR7g-efJ8GthvCwzvxOHHI3bLtCHyQBLXsAScoxsQiwXXNQ==

String receivedMessage = "";

// bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc);

void setup()
{
  Serial.begin(9600, SERIAL_8N1);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TXD2));
  Serial.println("Serial Rxd is on pin: " + String(RXD2));
  
}

void loop()
{
  while (Serial2.available()) {
    String receivedMessage = Serial2.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(receivedMessage);
  }
  delay(10); // Add a small delay
}

// bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc) {
//   DeserializationError error = deserializeJson(doc, receivedMessage);
//   if (error) {
//     Serial.print(F("deserializeJson() failed: "));
//     Serial.println(error.f_str());
//     return false;
//   }

//   if (!doc["deviceID"].is<const char*>()) {
//     Serial.println(F("Key 'deviceID' not found in JSON"));
//     return false;
//   }

//   return true;
// }
