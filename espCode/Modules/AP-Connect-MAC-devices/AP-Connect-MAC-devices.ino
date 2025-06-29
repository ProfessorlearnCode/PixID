#include <WiFi.h>
#include "esp_wifi.h"
#include <WiFi.h>
#include "esp_wifi.h"

void listConnectedStations() {
  wifi_sta_list_t stationList;
  esp_err_t err = esp_wifi_ap_get_sta_list(&stationList);
  if (err != ESP_OK) {
    Serial.println("Failed to get station list");
    return;
  }

  Serial.printf("Connected devices: %d\n", stationList.num);

  for (int i = 0; i < stationList.num; ++i) {
    wifi_sta_info_t station = stationList.sta[i];
    Serial.printf("Device %d MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1,
                  station.mac[0], station.mac[1], station.mac[2],
                  station.mac[3], station.mac[4], station.mac[5]);
  }
}

void setup() {
  Serial.begin(115200);

  const char* ssid = "ESP32-CAM-AP";
  const char* password = "12345678";

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  delay(10000); // Give time for devices to connect
  listConnectedStations();
}

void loop() {
  // Optional: Check periodically
  delay(10000);
  listConnectedStations();
}
