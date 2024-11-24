#include <Arduino.h>

#define RXD2 16
#define TXD2 17

String receivedMessage = "";

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
      Serial.print("You sent: ");
      Serial.println(receivedMessage);

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
