#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include <HTTPClient.h>

// Camera pin config
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5
#define Y2_GPIO_NUM    11
#define Y3_GPIO_NUM    9
#define Y4_GPIO_NUM    8
#define Y5_GPIO_NUM    10
#define Y6_GPIO_NUM    12
#define Y7_GPIO_NUM    18
#define Y8_GPIO_NUM    17
#define Y9_GPIO_NUM    16
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13

// LEDs & IR
#define GREEN_LED 2
#define RED_LED 1
#define IR_SENSOR_PIN 14  // IR sensor input pin

// Wi-Fi & server
const char* ssid = "esp32";
const char* password = "123456789";
bool devicePreviouslyConnected = false;

String host_ip = "";
const char* host_port = "5000";
WebServer server(80);

// ========== Helper Functions ==========

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

void blinkLED(int pin, int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayTime);
    digitalWrite(pin, LOW);
    delay(delayTime);
  }
}

void blinkBothLEDs(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    delay(delayTime);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    delay(delayTime);
  }
}

// ========== Camera Upload Logic ==========

void captureAndSendImage(bool viaGesture = false) {
  digitalWrite(RED_LED, HIGH);  // LED on

  // Discard first few frames
  for (int i = 0; i < 3; i++) {
    camera_fb_t* temp = esp_camera_fb_get();
    if (temp) esp_camera_fb_return(temp);
    delay(30);
  }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    blinkLED(RED_LED, 2, 300);
    digitalWrite(RED_LED, LOW);
    return;
  }

  delay(200);

  WiFiClient client;
  HTTPClient http;

  String serverURL = "http://" + host_ip + ":" + host_port + "/upload";
  http.begin(client, serverURL);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
    Serial.println(viaGesture ? "✅ Gesture Photo Sent!" : "✅ Manual Photo Sent!");
  } else {
    Serial.println("❌ Send Failed");
    blinkBothLEDs(3, 300);
  }

  esp_camera_fb_return(fb);
  http.end();
  digitalWrite(RED_LED, LOW);  // LED off
}

// ========== Web Routes ==========

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
  captureAndSendImage(false);
  server.send(200, "text/plain", "Photo captured and sent.");
}

void handleRegister() {
  IPAddress clientIP = server.client().remoteIP();
  host_ip = clientIP.toString();
  Serial.print("📲 Registered host IP: ");
  Serial.println(host_ip);
  server.send(200, "text/plain", "Registered host IP: " + host_ip);
}

// ========== Setup & Loop ==========

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  delay(1000);

  WiFi.softAP(ssid, password);
  delay(100);
  Serial.println("ESP32 AP started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  if (!setupCamera()) {
    Serial.println("Camera init failed 😢");
    blinkLED(RED_LED, 2, 300);
    return;
  }

  Serial.println("Camera ready 🎥");

  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.on("/register", handleRegister);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int connectedDevices = WiFi.softAPgetStationNum();

  if (connectedDevices >= 1 && !devicePreviouslyConnected) {
    blinkLED(GREEN_LED, 3, 300);
    devicePreviouslyConnected = true;
  }

  server.handleClient();

  // 👋 Gesture Detection
  if (digitalRead(IR_SENSOR_PIN) == HIGH) {
    Serial.println("👋 Gesture detected via IR sensor");
    blinkLED(RED_LED, 3, 150);       // Flash before capture
    captureAndSendImage(true);       // Capture via gesture
    delay(5000);                     // Debounce (prevent multiple triggers)
  }
}
