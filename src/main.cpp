#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define RXD2 16
#define TXD2 17

String receivedMessage = "";

bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc);

void setup()
{
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(9600);

  // Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TXD2));
  Serial.println("Serial Rxd is on pin: " + String(RXD2));
}

void loop()
{
  // Choose Serial1 or Serial2 as required
  while (Serial2.available())
  {
    char inCommingChar = char(Serial2.read());

    if (inCommingChar == '\n')
    {
      // Print the message
      Serial.print("You received: ");
      Serial.println(receivedMessage);
      
      // Parse the received message
      JsonDocument doc;
      if (parseReceivedMessage(receivedMessage, doc)) {
        // Successfully parsed JSON and 'deviceID' key exists
        // You can now use the `doc` object to access the JSON data
        String deviceID = doc["deviceID"].as<String>();
        Serial.print("deviceID: ");
        Serial.println(deviceID);

      }
      else {
        // Failed to parse JSON or 'deviceID' key does not exist
        // Handle the error here
        Serial.println("Failed to parse JSON or 'deviceID' key does not exist");
      }
      // Clear the message buffer for the next input
      receivedMessage = "";
    }
    else
    {
      // Append the character to the message string
      receivedMessage += inCommingChar;
    }
  }
}

bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc) {
  DeserializationError error = deserializeJson(doc, receivedMessage);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }

  if (!doc["deviceID"].is<const char*>()) {
    Serial.println(F("Key 'deviceID' not found in JSON"));
    return false;
  }

  return true;
}
