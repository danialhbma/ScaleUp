// wifi configuration file
// defines the SSID and password for wifi network

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// replace with your own wifi ssid and password before running
char* WIFI_SSID = "WIFI_SSID";
char* WIFI_PWD = "WIFI_PWD";

void connectToWifi(){
  // connect to wifi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PWD); 

  // keep looping if wifi is not connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println(" Connected!");
}

#endif
