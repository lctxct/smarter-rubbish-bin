// Obtained (and modified) from: https://github.com/flespi-software/ESP32-CAM/tree/main

#include <SPIFFS.h>

#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

#include <base64.h>

#include "ESP32MQTTClient.h"

#define SERVER "mqtt://0.0.0.0:1883"
#define SSID ""
#define PASSWD ""

// WiFi
const char ssid[] = SSID;
const char pass[] = PASSWD;

// MQTT
ESP32MQTTClient mqttClient;
const char server[] = SERVER;
char *subscribeTopic = "foo";
char* publishPhoto = "camera/photo";

// Still need? yes right?
framesize_t resolution_ = FRAMESIZE_QVGA;

#define SLEEP_DELAY 60000 // Delay for 60 Sec, 0 sec - PIR sensor mode

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int LED_BUILTIN = 4;

void setup_camera() {
  // Turn off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
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

  if (psramFound()) {
    config.frame_size = resolution_  ;// FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
}

void publishTelemetry(String data) {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  mqttClient.publish(publishPhoto, data.c_str(), 0, false);
}

void capturePhoto( void ) {
  // Retrieve camera framebuffer
  camera_fb_t * fb = NULL;
  uint8_t* _jpg_buf = NULL;
  esp_err_t res = ESP_OK;
  size_t frame_size = 0;
  Serial.print("Capturing Image...");

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  delay(1000);                       // wait for a second
  fb = esp_camera_fb_get();
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off
  delay(1000);                       // wait for a second
  if (!fb) {
    Serial.println("Camera capture failed");
    res = ESP_FAIL;
  } 
  else {
    Serial.println("Done!");
    Serial.println(String("Size of the image...") + String(fb->len));
    if (fb->format != PIXFORMAT_JPEG) {
      Serial.println("Compressing");
      bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &frame_size);
      esp_camera_fb_return(fb);
      fb = NULL;
      if (!jpeg_converted) {
        Serial.println("JPEG compression failed");
        res = ESP_FAIL;
      }
    } 
    else {
      frame_size = fb->len;
      _jpg_buf = fb->buf;
      Serial.print("Publish photo...");

      publishTelemetry(base64::encode(_jpg_buf, fb->len));
      Serial.println("Done!");
      esp_camera_fb_return(fb);
    }
  }
  if (res != ESP_OK) {
    return;
  }

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);

  setup_camera();
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();

  // client.setServer(mqtt_server, mqtt_port);
  // if (!SPIFFS.begin(true)) {
  //   Serial.println("An Error has occurred while mounting SPIFFS");
  //   return;
  // }

  mqttClient.setURI(server);
  mqttClient.enableLastWillMessage("lwt", "I am going offline");
  mqttClient.setKeepAlive(30);
  mqttClient.loopStart();
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("PSRAM found: " + String(psramFound()));

  if (mqttClient.isConnected()) {
    capturePhoto();
  }

  if (SLEEP_DELAY == 0) {
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1);
    delay(1000);
    esp_deep_sleep_start();
  }
  else if (SLEEP_DELAY > 0) {
    Serial.println("Going to sleep now");
    delay(SLEEP_DELAY);
  }
}

void onConnectionEstablishedCallback(esp_mqtt_client_handle_t client)
{
    if (mqttClient.isMyTurn(client)) // can be omitted if only one client
    {
        mqttClient.subscribe(subscribeTopic, [](const String &topic, const String &payload) {
          Serial.print(topic);
          Serial.print(": ");
          Serial.println(payload);
        });
    }
}

esp_err_t handleMQTT(esp_mqtt_event_handle_t event)
{
    mqttClient.onEventCallback(event);
    return ESP_OK;
}
