#include <WiFi.h>

#define LED_PIN 9

const char* ssid = "ESP32";
const char* password = "12345678";

void setup() {
  pinMode(LED_PIN, OUTPUT);


  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(100);

  Serial.println("Starting an ESP32 Access point 📍");
  Serial.print("IP Address 📡: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  int connectedDevices = WiFi.softAPgetStationNum();
  Serial.print("Connected Devices: ");
  Serial.println(connectedDevices);

  if (connectedDevices >= 1) {
    // Blink 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);
      delay(300);
    }
    delay(2000); // Wait before checking again
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(1000); // Wait before checking again
}
