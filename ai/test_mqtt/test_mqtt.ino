#include <WiFi.h>
#include "esp_wpa2.h"
#include "ESP32MQTTClient.h"

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

// ===== config =====
// Set as 1 if using school wifi 
#define SCHOOL_WIFI 1
// ===== mqtt secrets =====
// Replace ip address with ip of your windows 
// Click "properties" of the wifi you're currently connected to 
// and look for the ipv4 address
#define SERVER "mqtt://0.0.0.0:1883"
// ===== wifi secrets =====
#define SSID ""
#define PASSWD ""
// ===== school wifi secrets =====
#define NUS_NET_IDENTITY "nusstu\e"  //ie nusstu\e0123456
#define NUS_NET_USERNAME ""
#define NUS_NET_PASSWORD ""

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

// mqtt 
ESP32MQTTClient mqttClient;
const char server[] = SERVER;

char subscribeTopic[] = "classification";
char publishRecyclable[] = "fill_level/recyclable";
char publishTrash[] = "fill_level/trash";

#ifdef SCHOOL_WIFI
 const char ssid[] = "NUS_STU";
#else
  const char ssid[] = SSID;
  const char passwd[] = PASSWD;
#endif

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


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#ifdef SCHOOL_WIFI
    WiFi.begin(ssid, WPA2_AUTH_PEAP, NUS_NET_IDENTITY, NUS_NET_USERNAME, NUS_NET_PASSWORD); 
#else
    WiFi.begin(ssid, pass);
#endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}


void setup()
{
  Serial.begin(115200);
  setup_camera();
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
//     // ========== Initialize serial ==========
//     Serial.begin(115200);
//     // =========== Initialize WiFi ===========
//     Serial.print(F("Connecting to network: "));
//     Serial.println(ssid);
//     WiFi.disconnect(true);
// #ifdef SCHOOL_WIFI
//     WiFi.begin(ssid, WPA2_AUTH_PEAP, NUS_NET_IDENTITY, NUS_NET_USERNAME, NUS_NET_PASSWORD); 
// #else
//     WiFi.begin(ssid, pass);
// #endif
//     while (WiFi.status() != WL_CONNECTED) {
//       delay(300);
//       Serial.print(".");
//     }
//     Serial.println();
//     Serial.println("Connected to WiFi!");
//     WiFi.setHostname("c3test");
    // =========== Initialize MQTT ============
    // mqttClient.enableDebuggingMessages();
    mqttClient.setURI(server);
    mqttClient.enableLastWillMessage("lwt", "I am going offline");
    mqttClient.setKeepAlive(30);
    mqttClient.loopStart();
}

void loop()
{
    mqttClient.publish(publishRecyclable, "hellow", 0, false);

    delay(1000);
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
