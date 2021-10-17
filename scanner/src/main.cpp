#include "BarcodeReader.h"
#include "StatusLeds.h"
#include <AsyncMqttClient.h>
#include <M5Atom.h>
#include <Ticker.h>
#include <WiFi.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define MQTT_HOST IPAddress(192, 168, 1, 123)
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
BarcodeReader barcodeReader;
StatusLeds statusLeds(AppState::INIT);

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  statusLeds.setState(AppState::CONNECTING_WIFI);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  statusLeds.setState(AppState::CONNECTING_MQTT);
  mqttClient.connect();
}

void onWifiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    xTimerStop(
        mqttReconnectTimer,
        0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
    break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  mqttClient.subscribe("grocery-beep/product-add", 0);
  mqttClient.subscribe("grocery-beep/product-error", 0);
  statusLeds.setState(AppState::READY_TO_SCAN);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttMessage(char *topic, char *payload,
                   AsyncMqttClientMessageProperties properties, size_t len,
                   size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);

  if (String(topic) == "grocery-beep/product-add") {
    statusLeds.setState(AppState::PRODUCT_ADDED);
    delay(2000);
    statusLeds.setState(AppState::READY_TO_SCAN);
  } else if (String(topic) == "grocery-beep/product-error") {
    statusLeds.setState(AppState::PRODUCT_NOT_FOUND);
    delay(2000);
    statusLeds.setState(AppState::READY_TO_SCAN);
  }
}

void onBarcodeScan(const char *barcode) {
  Serial.println("New barcode scanned:");
  Serial.println(barcode);

  statusLeds.setState(AppState::BARCODE_SCANNED);

  uint16_t packetIdPub =
      mqttClient.publish("grocery-beep/barcode-scan", 0, true, barcode);

  Serial.print("MQTT packet published: ");
  Serial.println(packetIdPub);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  M5.begin(false, false, true);

  statusLeds.begin();

  mqttReconnectTimer =
      xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer =
      xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
                   reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(onWifiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();

  barcodeReader.onBarcodeScan(onBarcodeScan);
  barcodeReader.setup();
}

void loop() {
  M5.update();
  barcodeReader.loop();
}