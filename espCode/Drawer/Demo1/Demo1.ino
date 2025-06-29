#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include <HTTPClient.h>

// Replace with your actual camera pin mapping if different
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13


const char* ssid = "ESP";
const char* password = "12345678";

WebServer server(80);


// Note to self: First connect to the ESP hotspot and then "ipconfig" and copy the IPv4 address below
const char* host_ip = "192.168.4.2"; 
const int host_port = 5000;

bool setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_XGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  return (err == ESP_OK);
}

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
      <p><button onclick="capture()">Capture Photo</button></p>
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
  // camera_fb_t* fb = esp_camera_fb_get();
  
  // Flush 2–3 old frames to get the most recent scene
  for (int i = 0; i < 3; i++) {
    camera_fb_t* temp = esp_camera_fb_get();
    if (temp) {
      esp_camera_fb_return(temp);  // release buffer
    }
    delay(30);  // let the camera refresh (adjust if needed)
  }

  // Now grab the actual current frame
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    return;
  }

  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  String serverURL = String("http://") + host_ip + ":" + host_port + "/upload";
  http.begin(client, serverURL);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);
  String response = "Photo Sent! Response Code: " + String(httpResponseCode);

  esp_camera_fb_return(fb);
  http.end();

  server.send(200, "text/plain", response);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.softAP(ssid, password);
  delay(100);
  Serial.println("ESP32 AP started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  if (!setupCamera()) {
    Serial.println("Camera init failed 😢");
    return;
  }
  Serial.println("Camera ready 🎥");

  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
