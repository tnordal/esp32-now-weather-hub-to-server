#ifndef DB_H
#define DB_H

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <ArduinoJson.h>
#include <Arduino.h>

// InfluxDB configuration
#define INFLUXDB_URL "http://192.168.3.33:8090"
#define INFLUXDB_TOKEN "MSIMfBObMSIO0XNBugBkfoI6sn4EXIhfROQP3saiIox7PW_JN847HQO94GuPKUIWUQRrhtcXuhCC4-gQ1FB-9Q=="
#define INFLUXDB_ORG "41e02a6d59ffcd45"
#define INFLUXDB_BUCKET "first"
#define WRITE_PRECISION WritePrecision::S
#define MAX_BATCH_SIZE 10
#define WRITE_BUFFER_SIZE 30
#define TZ_INFO "UTC1"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
extern InfluxDBClient client;

// Declare Data point
extern Point sensor;

Point makePoint(String message);
void updateInfluxDB(Point point);
void setupInfluxDB();

#endif // DB_H