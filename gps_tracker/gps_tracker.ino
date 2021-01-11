#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;
static const int light = 0; // D3
// repace your wifi username and password
const char* ssid     = "whyred";
const char* password = "mengapamerah";

unsigned long myChannelNumber = 1021883;
const char * myWriteAPIKey = "5K6N47JE111GS31A";
const char * myReadAPIKey = "XH7HIU0RIZNJZ6IO";

unsigned int field3 = 3;

// The TinyGPS++ object
TinyGPSPlus gps;
WiFiClient  client;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); //--> LIGHT/Relay port Direction output
  pinMode(light, OUTPUT); //--> LIGHT/Relay port Direction output
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
  digitalWrite(light, LOW); //--> Turn off LIGHT. LIGHT/Relay use active high, some relay modules may be different.

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  ThingSpeak.begin(client);

}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
      relay();
    }
  }
}

void relay() {
  int statusCode = 0;
  int last_light_state = ThingSpeak.readFloatField(myChannelNumber, field3, myReadAPIKey);  //--> get the latest data from the fields on ThingSpeak
  statusCode = ThingSpeak.getLastReadStatus();  //--> Check the status of the read operation to see if it was successful
  if (statusCode == 200) {
    if (last_light_state == 1) {
      digitalWrite(light, HIGH);
      Serial.println("LIGHT is On");
    }
    else if (last_light_state == 0) {
      digitalWrite(light, LOW);
      Serial.println("LIGHT is Off");
    }
    Serial.print("The latest data from Field1 on ThingSpeak is : ");
    Serial.println(last_light_state);
  }
  else {
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode));
  }
}

void displayInfo()
{
  // Serial.print(F("Location: "));
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());

    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.println(latbuf);

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);

    ThingSpeak.setField(1, latbuf);
    ThingSpeak.setField(2, lonbuf);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
//    delay(20000);

  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
