#include <WiFi.h>
#include <WebServer.h> // For HTTP web server

const char* ssid = "ESP32_CAM_AP";
const char* password = "12345678";

WebServer server(80); // HTTP server on port 80

const int LED_PIN = 13;

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 CAM Control</title>
      <style>
        body { font-family: Arial; text-align: center; padding: 50px; }
        button { font-size: 20px; padding: 10px 20px; }
      </style>
    </head>
    <body>
      <h1>ESP32 CAM Interface</h1>
      <p><button onclick="capture()">📸 Capture Photo</button></p>
      <script>
        function capture() {
          fetch("/capture").then(res => res.text()).then(alert);
        }
      </script>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleCapture() {
  // Placeholder logic
  Serial.println("📸 Capture button clicked!");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "Capture Triggered (not implemented yet)");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.softAP(ssid, password);
  delay(100);

  Serial.println("ESP32 AP started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
