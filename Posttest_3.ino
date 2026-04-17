#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// ================= WIFI =================
const char* ssid = "Praktikum";
const char* password = "12344321";

// ================= MQTT =================
const char* mqtt_server = "broker.emqx.io";
WiFiClient espClient;
PubSubClient client(espClient);

// ================= PIN =================
#define WATER_SENSOR 34
#define BUZZER 16
#define SERVO_PIN 17

Servo myServo;

// ================= VARIABLE =================
String mode = "Manual";
String buzzerState = "OFF";
String servoState = "OFF";

int waterValue = 0;
String statusLevel = "AMAN";

// ================= TIMER =================
unsigned long lastSensor = 0;
unsigned long lastPublish = 0;
unsigned long lastBlink = 0;

const long intervalSensor = 1000;
const long intervalPublish = 1000;
const long intervalBlink = 100;

bool buzzerBlinkState = false;

// ================= SERIAL TRIGGER =================
bool updateSerial = true;

// ================= WIFI =================
void setup_wifi() {
  Serial.println("Menghubungkan WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());
}

// ================= MQTT CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  msg.trim();

  String t = String(topic);

  Serial.println("\nMQTT Masuk: " + t + " = " + msg);

  if (t == "wan/iot/kontrol") {
    mode = msg;
    updateSerial = true;
  }

  if (t == "wan/iot/kontrol_buzzer") {
    buzzerState = msg;
    updateSerial = true;
  }

  if (t == "wan/iot/kontrol_servo") {
    servoState = msg;
    updateSerial = true;
  }

  if (t == "wan/iot/update") {
    updateSerial = true;
  }
}

// ================= MQTT =================
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting MQTT...");

    if (client.connect("ESP32_FINAL_FIX")) {
      Serial.println("MQTT Connected!");
      client.subscribe("wan/iot/#");
      client.publish("wan/iot/koneksi", "Terhubung");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  pinMode(WATER_SENSOR, INPUT);

  myServo.attach(SERVO_PIN);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ================= LOOP =================
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  unsigned long now = millis();

  // ================= SENSOR =================
  if (now - lastSensor > intervalSensor) {
    lastSensor = now;

    waterValue = analogRead(WATER_SENSOR);

    // ===== HITUNG STATUS (SELALU ADA) =====
    if (waterValue <= 800) {
      statusLevel = "AMAN";
    } 
    else if (waterValue <= 1500) {
      statusLevel = "WASPADA";
    } 
    else {
      statusLevel = "BAHAYA";
    }

    // ===== MODE OTOMATIS =====
    if (mode.equalsIgnoreCase("automatis")) {

      if (statusLevel == "AMAN") {
        myServo.write(0);
        digitalWrite(BUZZER, LOW);
      } 
      else if (statusLevel == "WASPADA") {
        myServo.write(90);
        digitalWrite(BUZZER, LOW);
      } 
      else {
        myServo.write(180);

        
        if (now - lastBlink > intervalBlink) {
          lastBlink = now;
          buzzerBlinkState = !buzzerBlinkState;
          digitalWrite(BUZZER, buzzerBlinkState);
        }
      }
    }

    // ===== MODE MANUAL =====
    else {

      digitalWrite(BUZZER, buzzerState == "ON");
      myServo.write(servoState == "ON" ? 180 : 0);
    }
  }

  // ================= MQTT =================
  if (client.connected() && now - lastPublish > intervalPublish) {
    lastPublish = now;

    int sudut = (mode.equalsIgnoreCase("automatis")) ?
      (waterValue <= 800 ? 0 : (waterValue <= 1500 ? 90 : 180)) :
      (servoState == "ON" ? 180 : 0);

    client.publish("wan/iot/air", String(waterValue).c_str());
    client.publish("wan/iot/air2", statusLevel.c_str()); 
    client.publish("wan/iot/buzzer", digitalRead(BUZZER) ? "ON" : "OFF");
    client.publish("wan/iot/servo", String(sudut).c_str());
  }

  // ================= SERIAL =================
  if (updateSerial) {
    updateSerial = false;

    int sudut = (mode.equalsIgnoreCase("automatis")) ?
      (waterValue <= 800 ? 0 : (waterValue <= 1500 ? 90 : 180)) :
      (servoState == "ON" ? 180 : 0);

    Serial.println("\n===== STATUS SISTEM =====");
    Serial.println("Mode    : " + mode);
    Serial.println("Status  : " + statusLevel);
    Serial.println("Buzzer  : " + String(digitalRead(BUZZER) ? "ON" : "OFF"));
    Serial.println("Servo   : " + String(sudut));
  }
}