#include <Arduino.h>

// WiFi Server stuff and WiFi logon Credentials
const char *SSID = "???????";
const char *PASSWORD = "????????????";

// OTA stuff included here
String projectTitle = "Beehive Counter";
String projectDesc =
    "ESP32 for YouTube demonstration! <p>You can write text here or <b>html</b>.</p> \
  <p>Note: Try this on another ESP32 device before demo. And other important info. Keep it concise!</p>";
#include "Identification.h"

// Flashy flashy LED to prove we can change flash rate
#define ledPin 22 // For TTGO T7 V1.3 MINI

void setup()
{
    // Flashy flashy LED for our demo
    pinMode(ledPin, OUTPUT);

    // Everything for OTA updates
    otaInit();
}

// Nothing to see here. No, really.
void loop()
{
    // Variables just for this function
    static unsigned long counter = 1;
    static unsigned long prevMillis = 0;

    // We'll update a counter every few seconds
    if (millis() - prevMillis > 3000)
    {
        printf("Counter is now %ld\n", ++counter);
        prevMillis = millis();
    }

    // We'll flash an LED for n milliseconds
    digitalWrite(ledPin, millis() % 1000 > 975 ? HIGH : LOW);
}