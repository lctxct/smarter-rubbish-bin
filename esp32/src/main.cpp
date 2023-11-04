#include "Arduino.h"
#include <WiFi.h>
#include "esp_wpa2.h"
#include "ESP32MQTTClient.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#include "secrets.h"
#include "wifi_secrets.h"

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

const int triggerPin_1 = 27;
const int echoPin_1 = 26;
const int triggerPin_2 = 25;
const int echoPin_2 = 33;

float fill_level_1 = 0;
float fill_level_2 = 0;

Servo platform;
int pos = 0;
const int servoPin = 23;

LiquidCrystal_I2C lcd(0x27, 16,2);  

float get_distance(int triggerPin, int echoPin);

void setup()
{
    // ========== Initialize serial ==========
    Serial.begin(115200);
    // ========== Initialize ultrasonic ==========
    pinMode(triggerPin_1, OUTPUT);
    pinMode(echoPin_1, INPUT);
    pinMode(triggerPin_2, OUTPUT);
    pinMode(echoPin_2, INPUT);
    // ========== Initialize servo ==========
    ESP32PWM::allocateTimer(1);   
    platform.setPeriodHertz(50);
    platform.attach(servoPin, 500, 2400);
    // ========== Initialize LCD ==========
    lcd.init();
    lcd.backlight();
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
    // mqttClient.publish(publishRecyclable, "hellow", 0, false);

    // read ultrasonic sensor -- when actuation
    fill_level_1 = get_distance(triggerPin_1, echoPin_1);
    fill_level_2 = get_distance(triggerPin_2, echoPin_2);

    Serial.print("Ultrasonic sensor 1: ");
    Serial.println(fill_level_1);
    Serial.print("Ultrasonic sensor 2: ");
    Serial.println(fill_level_2);

    mqttClient.publish(publishRecyclable, String(fill_level_1), 0, false);
    mqttClient.publish(publishTrash, String(fill_level_2), 0, false);

    // LCD -- idle
    lcd.setCursor(0, 1); 
    if (fill_level_1 > fill_level_2) {
      lcd.print(fill_level_1);
    } else {
      lcd.print(fill_level_2);
    }

    platform.write(0); delay(1000);
    platform.write(180); delay(1000);

    delay(2000);
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

float get_distance(int triggerPin, int echoPin) {

  // Clears the trigger pin first
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Must send 10us pulse to trigger pin to work properly
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Reads the echoPin, returns sound wave travel time in ms
  long duration = pulseIn(echoPin, HIGH);

  // Speed of sound 340m/s
  return (0.034 * duration) / 2; 
}
