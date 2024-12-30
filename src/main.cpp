#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include "secrets.h"

#define INFLUXDB_URL "http://192.168.3.33:8090"
#define INFLUXDB_TOKEN "MSIMfBObMSIO0XNBugBkfoI6sn4EXIhfROQP3saiIox7PW_JN847HQO94GuPKUIWUQRrhtcXuhCC4-gQ1FB-9Q=="
#define INFLUXDB_ORG "41e02a6d59ffcd45"
#define INFLUXDB_BUCKET "first"

#define WRITE_PRECISION WritePrecision::S
#define MAX_BATCH_SIZE 10
#define WRITE_BUFFER_SIZE 30

// Time zone info
#define TZ_INFO "UTC1"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("wifi_status");

#define RXD2 16
#define TXD2 17

// 2UYef3lh9yhbrmBMaGW2VeHo6Q_Adfk4G5X1IE7b3ZREw-Hd3JXkpl-Ig8I8DK_m6Ox7PknX7tdmMFgW9OIZrw==

String receivedMessage = "";

// bool parseReceivedMessage(const String& receivedMessage, JsonDocument& doc);

Point makePoint(String message)
{
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return Point("error");
  }

  const char *measurement = doc["measurement"];
  const char *location = doc["tags"]["location"];
  const char *type = doc["tags"]["type"];
  const char *sensor_id = doc["tags"]["sensor_id"];
  float value = doc["fields"]["value"];

  Point point(measurement);
  point.addTag("location", location);
  point.addTag("type", type);
  point.addTag("sensor_id", sensor_id);
  point.addField("value", value);

  // Print the point for debugging
  Serial.print("Point created: ");
  Serial.println(point.toLineProtocol());

  return point;
}

void updateInfluxDB(Point point)
{
  // Write point to InfluxDB
  if (!client.writePoint(point))
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

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

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  }
  else
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  // Enable messages batching and retry buffer
  client.setWriteOptions(WriteOptions().writePrecision(WRITE_PRECISION).batchSize(MAX_BATCH_SIZE).bufferSize(WRITE_BUFFER_SIZE));
}

void loop()
{
  while (Serial2.available())
  {
    String receivedMessage = Serial2.readStringUntil('\n');
    // Serial.print("Received: ");
    // Serial.println(receivedMessage);
    Point p = makePoint(receivedMessage);
    // check if the point is valid
    if (p.toLineProtocol() != "error")
    {
      updateInfluxDB(p);
    }
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
