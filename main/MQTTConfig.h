// paste the MQTTConfig.txt file contents here
#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

const char* MQTTServer = "";
const int MQTTPort = 8883; // Replace with your MQTT broker's port
const char* MQTTUsername = "";
const char* MQTTPassword = "";
const char* MQTTClientID = "ESP32_Client";
const char* MQTTTopic = ""; // Replace with the MQTT topic you want to publish/subscribe to

const char* MQTTCACert =R"EOF(
  -----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";


#endif
