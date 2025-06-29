#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include <HTTPClient.h>

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

#define GREEN_LED 2
#define RED_LED 1


const char* ssid = "ESP_32";
const char* password = "123456789";
// bool devicePreviouslyConnected = false;


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

  // Turn red LED on to indicate capture in progress
  digitalWrite(RED_LED, HIGH);
  
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
    blinkLED(RED_LED, 2, 300);  // Blink red 2 times on camera failure
    server.send(500, "text/plain", "Camera capture failed");
    digitalWrite(RED_LED, LOW);  // Ensure LED off
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
  if (httpResponseCode > 0) {
    Serial.println("✅ Photo Sent!");
  } else {
    Serial.println("❌ Send Failed");
    blinkBothLEDs(3, 300);  // Blink both LEDs if send failed
  }

  String response = "Photo Sent! Response Code: " + String(httpResponseCode);

  esp_camera_fb_return(fb);
  http.end();

  server.send(200, "text/plain", response);
  
  // Turn red LED off after process
  digitalWrite(RED_LED, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

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
    blinkLED(RED_LED, 2, 300);  // Add this line to blink red on camera error
    return;
  }
  Serial.println("Camera ready 🎥");

  server.on("/", handleRoot);
  server.on("/capture", handleCapture);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int connectedDevices = WiFi.softAPgetStationNum();

  if (connectedDevices >= 1) {
    blinkLED(GREEN_LED, 3, 300);  // Blink green 3 times
    // devicePreviouslyConnected = true; // Track state to avoid re-blinking
  }

  server.handleClient();
}
