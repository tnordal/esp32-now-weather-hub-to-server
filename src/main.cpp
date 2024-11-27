#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include "secrets.h"

const char MQTT_BROKER_ADRRESS[] = "192.168.3.33";
const uint16_t MQTT_PORT = 1883;
const char PUBLISH_WEATHER_OUTDOOR_TOPIC[] = "weather/outdoor/";
const char PUBLISH_WEATHER_INDOOR_TOPIC[] = "weather/indoor/";

// Create WiFi and MQTT client instances
WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define RXD2 16
#define TXD2 17

String receivedMessage = "";

void setupWiFi();
bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc);
void reconnectMqttClient();

void setup()
{
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);

  // Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TXD2));
  Serial.println("Serial Rxd is on pin: " + String(RXD2));

  setupWiFi();

  mqttClient.setServer(MQTT_BROKER_ADRRESS, MQTT_PORT);

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
      Serial.println();
      
      // Parse the received message
      JsonDocument doc;
      if (parseReceivedMessage(receivedMessage, doc)) {
        // Successfully parsed JSON and 'deviceID' key exists
        // You can now use the `doc` object to access the JSON data
        String deviceID = doc["deviceID"].as<String>();
        Serial.print("deviceID: ");
        Serial.println(deviceID);

        // Publish the data to the MQTT broker
        if (deviceID == "ESP32-1-outdoor")
        {
          // Publish the data to the MQTT broker
          if (!mqttClient.connected())
          {
            reconnectMqttClient();
          }
          // mqttClient.publish(PUBLISH_WEATHER_OUTDOOR_TOPIC, receivedMessage.c_str());
          mqttClient.publish(PUBLISH_WEATHER_OUTDOOR_TOPIC, receivedMessage.c_str());
          Serial.println("Published data to MQTT broker");
        }
        else if (deviceID == "ESP32-2-indoor")
        {
          // Publish the data to the MQTT broker
          if (!mqttClient.connected())
          {
            reconnectMqttClient();
          }
          // mqttClient.publish(PUBLISH_WEATHER_INDOOR_TOPIC, receivedMessage.c_str());
          mqttClient.publish(PUBLISH_WEATHER_INDOOR_TOPIC, receivedMessage.c_str());
          Serial.println("Published data to MQTT broker");
        }
        else
        {
          // Invalid deviceID
          Serial.println("Invalid deviceID");
        }      

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

void setupWiFi(){
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 2 * 60 * 1000; // 2 minutes in milliseconds

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startAttemptTime >= timeout)
        {
            Serial.println("Failed to connect to WiFi within 2 minutes. Rebooting...");
            ESP.restart();
        }
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  
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

void reconnectMqttClient()
{
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("ESP32Client"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying, use mills() to avoid blocking
            delay(5000);
        }
    }
}