// Libraries required 
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// custom config files 
#include "WifiConfig.h"
#include "MQTTConfig.h"

// START of setup section
// This section contains the setup for the LED strips & the number of ingredients. 
// To add more LED strips & ingredients, add the pin number for the LED strip & the number of LEDs on the strip. 
// Then add the ingredient name to the ingredientNames array.
// The ingredient name must be the same as the one in the excel sheet. Make sure there are no typos as well.

#define PIN_STRIP1    27  // strip 1 data pin
#define PIN_STRIP2    14  // strip 2 data pin
#define PIN_STRIP3    12  // strip 3 data pin
// refer to the ESP32-WROOM layout on the pins available for use

// defining the nuber of LEDs on each strip
#define NUMPIXELS_STRIP1   7
#define NUMPIXELS_STRIP2   7
#define NUMPIXELS_STRIP3   7
// make sure to define the number of LEDs on each new strip added

Adafruit_NeoPixel strip1(NUMPIXELS_STRIP1, PIN_STRIP1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(NUMPIXELS_STRIP2, PIN_STRIP2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(NUMPIXELS_STRIP3, PIN_STRIP3, NEO_GRB + NEO_KHZ800);
// add more LED strips as needed

const char* ingredientNames[] = {
  "All-purpose flour",
  "Unsalted butter",
  "Granulated sugar",
  // Add more ingredients as needed
};

// END of setup section

// required setup for MQTT
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

int activeStrip = -1; // Track the active LED strips

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message arrived [" + String(topic) + "]");

    // Convert the payload to a string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    bool ingredientMatched = false; // Flag to track if any ingredient matched

    // Check if the received message matches any ingredient name
    for (int i = 0; i < sizeof(ingredientNames) / sizeof(ingredientNames[0]); i++) {
        if (message.equalsIgnoreCase(ingredientNames[i])) {
            Serial.println("Received ingredient: " + message);
            
            // Turn off the currently active strip
            if (activeStrip == 1) strip1.clear();
            else if (activeStrip == 2) strip2.clear();
            else if (activeStrip == 3) strip3.clear();
            // add more conditions to check for any additional LED strips as needed

            if (i == 0) { // All-purpose flour
                strip1.fill(strip1.Color(0, 255, 0)); 
                activeStrip = 1;
            } else if (i == 1) { // Unsalted butter
                strip2.fill(strip2.Color(0, 255, 0)); 
                activeStrip = 2;
            } else if (i == 2) { // Granulated sugar
                strip3.fill(strip3.Color(0, 255, 0)); 
                activeStrip = 3;
            } // in the event of new ingredients, change to a switch statement to better handle more ingredients
            
            strip1.show();
            strip2.show();
            strip3.show();
            // make sure to add more show() functions for any additional LED strips. this ensures that the LED strips are updated
            
            ingredientMatched = true; // Set the flag to true when an ingredient is matched
            break; 
        }
    }

    // If no ingredient matched, turn off all the LED strips
    if (!ingredientMatched) {
        strip1.clear();
        strip2.clear();
        strip3.clear();
        strip1.show();
        strip2.show();
        strip3.show();
        // make sure to add more show() functions for any additional LED strips. this ensures that the LED strips are updated
        
        // Print the received message even if it doesn't match
        Serial.println("Received message: " + message);
    }

    Serial.println(); 
}

// MQTT reconnect function to test if HiveMQ is available
void reconnect() {
    while (!mqttClient.connected()) {
        Serial.println("Connecting to MQTT...");
        if (mqttClient.connect(MQTTClientID, MQTTUsername, MQTTPassword)) {
            Serial.println("Connected to MQTT");

            mqttClient.subscribe(MQTTTopic, 1); // qos 1
        } else {
            Serial.println("MQTT connection failed, retrying in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200); 
    // make sure to change the baud rate to 115200 in the serial monitor to receive the correct output

    connectToWifi();
    Serial.println("Initializing the inventory");

    // Initialize MQTT client
    mqttClient.setServer(MQTTServer, MQTTPort);
    mqttClient.setClient(espClient);
    espClient.setCACert(MQTTCACert);

    // set callback for published messages
    mqttClient.setCallback(callback);
    
    strip1.begin();
    strip2.begin();
    strip3.begin();
    // add more begin() functions for any additional LED strips as needed
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect(); 
    }

    mqttClient.loop(); 
}
