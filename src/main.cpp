#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

#include "secrets.h"
#include "display.h"
#include "db.h" // Include the db.h file

#define RXD2 16
#define TXD2 17

String receivedMessage = "";

void setup()
{
  Serial.begin(9600, SERIAL_8N1);
  setupDisplay(); // Call the setup function for the display

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TXD2));
  Serial.println("Serial Rxd is on pin: " + String(RXD2));

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  setupInfluxDB(); // Call the setup function for InfluxDB
  bootMessage();    // Call the boot message function
}

void loop()
{
  while (Serial2.available())
  {
    String receivedMessage = Serial2.readStringUntil('\n');
    Point p = makePoint(receivedMessage);
    // check if the point is valid
    if (p.toLineProtocol() != "error")
    {
      Serial.println("Received message: " + receivedMessage); // Print the received message
      updateInfluxDB(p);
      delay(20);
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, receivedMessage);
      if (!error) {
        JsonObject obj = doc.as<JsonObject>();
        displayJsonData(obj);
      } else {
        Serial.println("Failed to parse JSON");
      }
    }
  }
  delay(10); // Add a small delay
}