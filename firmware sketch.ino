
#include <WiFi.h>
#include <PubSubClient.h>

// Wokwi Wi-Fi
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// MQTT test broker
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

// Dashboard must use the same topic
const char* MQTT_TOPIC = "mathavi622/capstone/distance";

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int LED_PIN = 2;

const unsigned long SENSOR_INTERVAL = 5000;
const unsigned long WIFI_RETRY = 10000;
const unsigned long MQTT_RETRY = 5000;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

struct Reading {
  unsigned long id;
  float distance;
};

const int BUFFER_SIZE = 20;
Reading bufferData[BUFFER_SIZE];

int head = 0;
int tail = 0;
int bufferCount = 0;

unsigned long readingId = 0;
unsigned long lastSensorTime = 0;
unsigned long lastWiFiAttempt = 0;
unsigned long lastMQTTAttempt = 0;

void saveReading(float distance) {
  if (bufferCount == BUFFER_SIZE) {
    head = (head + 1) % BUFFER_SIZE;
    bufferCount--;

    Serial.println("Buffer full: oldest reading removed");
  }

  bufferData[tail].id = ++readingId;
  bufferData[tail].distance = distance;

  tail = (tail + 1) % BUFFER_SIZE;
  bufferCount++;

  Serial.print("Buffered readings: ");
  Serial.println(bufferCount);
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  unsigned long now = millis();

  if (lastWiFiAttempt == 0 ||
      now - lastWiFiAttempt >= WIFI_RETRY) {

    lastWiFiAttempt = now;

    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);
  }
}

void connectMQTT() {
  if (WiFi.status() != WL_CONNECTED ||
      mqtt.connected()) {
    return;
  }

  unsigned long now = millis();

  if (lastMQTTAttempt == 0 ||
      now - lastMQTTAttempt >= MQTT_RETRY) {

    lastMQTTAttempt = now;

    String clientId = "MathaviCapstone-" +
      String((uint32_t)ESP.getEfuseMac(), HEX);

    Serial.println("Connecting to MQTT...");

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(mqtt.state());
    }
  }
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
      pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  return duration * 0.0343f / 2.0f;
}

void publishReading() {
  if (!mqtt.connected() || bufferCount == 0) {
    return;
  }

  Reading r = bufferData[head];

  char payload[120];

  snprintf(
    payload,
    sizeof(payload),
    "{\"id\":%lu,\"distance_cm\":%.2f,\"uptime_ms\":%lu}",
    r.id,
    r.distance,
    millis()
  );

  if (mqtt.publish(MQTT_TOPIC, payload)) {
    Serial.print("Published: ");
    Serial.println(payload);

    head = (head + 1) % BUFFER_SIZE;
    bufferCount--;
  } else {
    Serial.println("Publish failed; reading kept");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setBufferSize(256);

  Serial.println("Smart Distance Monitoring Started");
}

void loop() {
  connectWiFi();
  connectMQTT();

  if (mqtt.connected()) {
    mqtt.loop();
  }

  unsigned long now = millis();

  if (now - lastSensorTime >= SENSOR_INTERVAL) {
    lastSensorTime = now;

    float distance = readDistance();

    if (distance >= 0) {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      if (distance <= 20) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }

      saveReading(distance);
    } else {
      Serial.println("Sensor timeout");
      digitalWrite(LED_PIN, LOW);
    }
  }

  publishReading();
}
