#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <ArduinoJson.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

// Remove the redefinitions of Wire and SPI
// TwoWire Wire(1);
// SPIClass SPI(0);

#define SCREEN_WIDTH 132 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 1

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

#define GMT_OFFSET_SEC 3600

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, GMT_OFFSET_SEC);

// Make a init function for the display, the address is 0x3C
void setupDisplay()
{
  // Serial.begin(115200); // Initialize serial communication for debugging
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
  delay(2000);
}

// Make a function to display a boot message and a splash screen
void bootMessage()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 32);
  display.println("InfluxDB Logger");
  display.display();
  delay(4000);
}

#define MAX_MESSAGES 5

struct Message
{
  char type[32];
  float value;
};

Message messages[MAX_MESSAGES];
int messageIndex = 0;

// Make a function to display the current time on top of the screen and show receivedMessage from serial port.
void displayMessage(String message)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Message: " + message);
  display.display();
}

// Function to display type and value on the OLED display
void displayJsonData(const JsonObject &doc)
{
  // Store the new message in the circular buffer
  strlcpy(messages[messageIndex].type, doc["tags"]["type"], sizeof(messages[messageIndex].type));
  messages[messageIndex].value = doc["fields"]["value"];
  messageIndex = (messageIndex + 1) % MAX_MESSAGES;

  // Clear the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  timeClient.update();
  display.setCursor(0, 0);
  display.print("Time: ");
  display.println(timeClient.getFormattedTime());
  display.display();

  // Display the messages in reverse order
  int y = 16;
  for (int i = 0; i < MAX_MESSAGES; i++)
  {
    int index = (messageIndex + MAX_MESSAGES - 1 - i) % MAX_MESSAGES;
    display.setCursor(0, y);
    display.print("Type: ");
    display.print(messages[index].type);
    display.print(" MV: ");
    display.println(messages[index].value);
    y += 8; // Move to the next line
  }

  // Update the display
  display.display();
}

// void 1displayJsonData(const JsonObject& doc) {
//   timeClient.update();
//   Serial.println(timeClient.getFormattedTime());

//   const char *type = doc["tags"]["type"];
//   float value = doc["fields"]["value"];

//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.print("Time: ");
//   display.println(timeClient.getFormattedTime());
//   display.print("Type: ");
//   display.print(type);
//   display.print(" MV: ");
//   display.print(value);
//   display.display();
// }