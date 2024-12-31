#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

#include "secrets.h"
#include "db.h" // Include the db.h file

#define RXD2 16
#define TXD2 17

String receivedMessage = "";

void setup()
{
  Serial.begin(9600, SERIAL_8N1);
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
      updateInfluxDB(p);
    }
  }
  delay(10); // Add a small delay
}