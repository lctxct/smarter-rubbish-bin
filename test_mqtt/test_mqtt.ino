#include <WiFi.h>
#include "esp_wpa2.h"
#include "ESP32MQTTClient.h"

// ===== config =====
// Set as 1 if using school wifi 
#define SCHOOL_WIFI 0
// ===== mqtt secrets =====
// Replace ip address with ip of your windows 
// Click "properties" of the wifi you're currently connected to 
// and look for the ipv4 address
#define SERVER "mqtt://0.0.0.0:1883"
// ===== wifi secrets =====
#define SSID ""
#define PASSWD ""
// ===== school wifi secrets =====
#define NUS_NET_IDENTITY "nusstu\exxxxxxx"  //ie nusstu\e0123456
#define NUS_NET_USERNAME "exxxxxx"
#define NUS_NET_PASSWORD ""

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
  const char pass[] = PASSWD;
#endif

void setup()
{
    // ========== Initialize serial ==========
    Serial.begin(115200);
    // =========== Initialize WiFi ===========
    Serial.print(F("Connecting to network: "));
    Serial.println(ssid);
    WiFi.disconnect(true);
#ifdef SCHOOL_WIFI
    WiFi.begin(ssid, WPA2_AUTH_PEAP, NUS_NET_IDENTITY, NUS_NET_USERNAME, NUS_NET_PASSWORD); 
#else
    WiFi.begin(ssid, pass);
#endif
    while (WiFi.status() != WL_CONNECTED) {
      delay(300);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to WiFi!");
    WiFi.setHostname("c3test");
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
