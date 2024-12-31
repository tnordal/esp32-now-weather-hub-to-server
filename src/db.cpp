#include "db.h"

// Define InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Define Data point
Point sensor("wifi_status");

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

void setupInfluxDB()
{
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
