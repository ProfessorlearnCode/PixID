#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"

#include "camera_config.h" 

const char* ssid = "esp";
const char* password = "password";

// AP network settings
IPAddress local_IP(192, 168, 1, 22);
IPAddress gateway(192, 168, 1, 5);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// Camera capture route
void handleCapture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

#include <HTTPClient.h>

// Flask server IP
const char* flask_server_url = "http://192.168.0.100:5000/upload";  // Replace with your Flask server IP

void sendImageToFlask(camera_fb_t* fb) {
  HTTPClient http;
  http.begin(flask_server_url);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);
  if (httpResponseCode > 0) {
    Serial.printf("Image sent! Response: %d\n", httpResponseCode);
  } else {
    Serial.printf("Failed to send image. Error: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}

void handleSend() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  sendImageToFlask(fb);
  esp_camera_fb_return(fb);

  server.send(200, "text/plain", "Image sent to Flask server");
}


void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32-CAM Capture</title>
      <style>
        body { font-family: sans-serif; text-align: center; background: #f0f0f0; }
        h1 { color: #333; }
        img { width: 320px; height: auto; border: 1px solid #ccc; margin-top: 20px; }
        button { padding: 10px 20px; margin-top: 20px; font-size: 16px; }
      </style>
      <script>
        function capture() {
          const img = document.getElementById('photo');
          img.src = '/capture?t=' + new Date().getTime();  // Prevent caching
        }
      </script>
    </head>
    <body>
      <h1>ESP32-CAM AP Server</h1>
      <button onclick="capture()">Capture Image</button>
      <button onclick="sendToFlask()">Send to Flask Server</button>
      <div>
        <img id="photo" src="/capture" alt="Captured image will appear here"/>
      </div>
    </body>
    <script>
      function capture() {
        const img = document.getElementById('photo');
        img.src = '/capture?t=' + new Date().getTime();  // Prevent caching
      }

      function sendToFlask() {
        fetch('/send')
          .then(response => response.text())
          .then(data => alert(data))
          .catch(err => alert("Failed to send image"));
      }
    </script>

    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}


void setup() {
  Serial.begin(115200);

  // Init green LED for connection indication
  pinMode(19, OUTPUT);
  pinMode(42, OUTPUT);

  digitalWrite(19, LOW);
  digitalWrite(42, LOW);

  

  // Start camera
  camera_config_t config = getCameraConfig();  // Defined in camera_config.h
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    digitalWrite(42, HIGH);
    return;
  }
  digitalWrite(42, LOW);

  // Set up AP
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Route
  server.on("/", HTTP_GET, handleRoot);
  server.on("/capture", HTTP_GET, handleCapture);
  server.on("/send", HTTP_GET, handleSend);


  server.begin();
}

void loop() {
  server.handleClient();

  // Blink LED when client connected
  int clients = WiFi.softAPgetStationNum();
  digitalWrite(19, clients > 0 ? HIGH : LOW);
}
