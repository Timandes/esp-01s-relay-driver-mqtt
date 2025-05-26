/*
   Copyright 2025 Timandes White

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing perm:\Arduino\Projects\wake-up\config.hmissions and
   limitations under the License.
 */
 

#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include<Ticker.h>
#include"config.h"

#ifdef HOME_ASSISTANT_MQTT_DISCOVER_ENABLED
#include"ArduinoJson.h"

void mqttTimeUp();

Ticker timer4MqttDiscovery;
#endif

void mqttStatusPubTimeUp();
Ticker timer4MqttStatusPub;

#define GPIO OUTPUT_PIN

// 定义板载LED的管脚
#ifdef D9
#define ON_BOARD_LED D9
#endif
#ifdef LED_BUILTIN
#define ON_BOARD_LED LED_BUILTIN
#endif

#define LOOP_INTERVAL 10

// 定义开关状态值
#define OFF_STATE CLOSE_STATE
#define ON_STATE !OFF_STATE

WiFiClient wiFiClient;
PubSubClient client(wiFiClient);


uint8_t lastOnBoardLightState = HIGH;
unsigned long lastOnBoardLightUpdatedAt = 0;
unsigned long onBoardLightBlinkInterval = 1000;

uint8_t toggleOnBoardLightState() {
  if (lastOnBoardLightState == HIGH) {
    return LOW;
  } else {
    return HIGH;
  }
}

void ledTo(uint8_t pin, uint8_t state) {
  if (digitalRead(pin) != state) {
    Serial.print("LED => ");
    Serial.println(state);
    digitalWrite(pin, state);
  }
}

void blink(uint8_t pin) {
  if (onBoardLightBlinkInterval <= 0) {
    ledTo(pin, LOW);
    return;
  }

  unsigned long now = millis();
  if (now - lastOnBoardLightUpdatedAt >= onBoardLightBlinkInterval) {
    //Serial.println(now);
    //Serial.println(lastOnBoardLightUpdatedAt);
    //Serial.println(now - lastOnBoardLightUpdatedAt);
    uint8_t newState = toggleOnBoardLightState();

    ledTo(pin, newState);

    lastOnBoardLightState = newState;
    lastOnBoardLightUpdatedAt = millis();
  }
}

void initWifiSta() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  Serial.print("Preparing Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
#ifdef ON_BOARD_LED
    blink(ON_BOARD_LED);
#endif
    delay(500);
  }
  Serial.println(WiFi.localIP());
}

String toString(byte *c, unsigned int len) {
  String retval;
  for (int i=0; i<len; ++i) {
    retval += (char)c[i];
  }
  return retval;
}

void turnOn() {
  digitalWrite(GPIO, ON_STATE);
  Serial.print("GPIO ");
  Serial.print(GPIO);
  Serial.print(" ");
  Serial.println(ON_STATE);
  pubStatus();
}

void turnOff() {
  digitalWrite(GPIO, OFF_STATE);
  Serial.print("GPIO ");
  Serial.print(GPIO);
  Serial.print(" ");
  Serial.println(OFF_STATE);
  pubStatus();
}

void switchOnComputer() {
  Serial.print("Switching on computer(");
  Serial.print(ON_STATE);
  Serial.print(" -> ");
  Serial.print(OFF_STATE);
  Serial.print(")...");

  turnOn();
  delay(200);
  turnOff();

  Serial.println("DONE");
}

void mqttMessageArrived(char *topic, byte *payload, unsigned int payloadLen) {
  String message = toString(payload, payloadLen);
  Serial.print("Got message: ");
  Serial.println(message);

  if (message == "ON" || message == "on") {
    turnOn();
  } else if (message == "OFF" || message == "off") {
    turnOff();
  } else if (message == "ComputerOn") {
#ifdef COMPUTER_ON_ENABLED
    switchOnComputer();
#endif
  }
}

String macToStr(const uint8_t* mac){
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void initMqtt() {
  client.setBufferSize(MQTT_PAYLOAD_BUF_SIZE);
  client.setServer(MQTT_HOST, 1883);
  client.setCallback(mqttMessageArrived);
}

String composeClientId() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  String clientId;
  clientId += "esp-";
  clientId += macToStr(mac);
  return clientId;
}

int connectToMqttServer() {
  Serial.println("Connecting to MQTT server...");

  onBoardLightBlinkInterval = 300;

  String clientId = composeClientId();
  Serial.print("With client ID: ");
  Serial.println(clientId);
  if (client.connect(clientId.c_str())) {

    onBoardLightBlinkInterval = 0;
    Serial.println("MQTT server connected");
    return 0;
  } else {
    Serial.println("Failed to connect to MQTT server, retrying...");

    Serial.print("IP Addr:");
    Serial.println(WiFi.localIP());

    onBoardLightBlinkInterval = 1000;

    int n = 5000 / LOOP_INTERVAL;
    for (int i=0;i<n;++i) {
#ifdef ON_BOARD_LED
      blink(ON_BOARD_LED);
#endif
      delay(LOOP_INTERVAL);
    }
    return 1;
  }
}

void subscribeMqttTopic() {
  Serial.print("Subscribing Topic: ");
  Serial.println(MQTT_COMMAND_TOPIC);

  if (client.subscribe(MQTT_COMMAND_TOPIC)) {
    Serial.println("Topic subsribed");
  } else {
    client.disconnect();
    Serial.println("Fail to subscribe, retrying in 10 sec(s)");
    delay(10000);
  }
}

void toggleGpio() {
  Serial.println("Toggling GPIO...");
  digitalWrite(GPIO, !digitalRead(GPIO));
}

void initGpio() {
  pinMode(GPIO, OUTPUT);
  digitalWrite(GPIO, OFF_STATE);
  //ticker.attach(1, toggleGpio);
}

void initLed(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void initHomeAssistantDevice() {
  String topic = HOME_ASSISTANT_TOPIC_PREFIX;
  topic += "/switch/";
  topic += HOME_ASSISTANT_OBJECT_ID;
  topic += "/config";
  
  Serial.print("Publishing to Topic: ");
  Serial.println(topic);

  JsonDocument doc;
  doc["name"] = serialized("null");
  doc["device_class"] = "switch";
  doc["state_topic"] = MQTT_STATUS_TOPIC;
  doc["command_topic"] = MQTT_COMMAND_TOPIC;
  doc["object_id"] = HOME_ASSISTANT_OBJECT_ID;
  doc["unique_id"] = HOME_ASSISTANT_OBJECT_ID;
  doc["device"]["identifiers"][0] = HOME_ASSISTANT_OBJECT_ID;
  doc["device"]["name"] = HOME_ASSISTANT_NAME;

  String payload = "";
  serializeJson(doc, payload);
  boolean r = client.publish_P(topic.c_str(), payload.c_str(), false);
  if (r) {
    Serial.print("Message published: ");
  } else {
    Serial.print("Message publishing failed: ");
  }
  Serial.println(payload);
  Serial.print("payload.len=");
  Serial.println(payload.length());
}

String readStatusString() {
  int status = digitalRead(GPIO);
  if (status == ON_STATE) {
    return "ON";
  } else {
    return "OFF";
  }
}

void pubStatus() {
  Serial.print("Publishing to Topic: ");
  Serial.println(MQTT_STATUS_TOPIC);

  String payload = readStatusString();
  boolean r = client.publish_P(MQTT_STATUS_TOPIC, payload.c_str(), false);
  if (r) {
    Serial.print("Message published: ");
  } else {
    Serial.print("Message publishing failed: ");
  }
  Serial.println(payload);
  Serial.print("payload.len=");
  Serial.println(payload.length());
}

void mqttTimeUp() {
  Serial.println("MQTT Discovery time up");
  initHomeAssistantDevice();
}

void mqttStatusPubTimeUp() {
  Serial.println("MQTT Status Pub time up");
  pubStatus();
}

void setup() {
  Serial.begin(115200);
  Serial.println("");

#ifdef ON_BOARD_LED
  initLed(ON_BOARD_LED);
#endif
  initGpio();
  initWifiSta();
  initMqtt();
  
#ifdef HOME_ASSISTANT_MQTT_DISCOVER_ENABLED
  timer4MqttDiscovery.attach_ms(HOME_ASSISTANT_TOPIC_PUBLISH_INTERVAL_IN_MILLIS, mqttTimeUp);
#endif
  timer4MqttStatusPub.attach_ms(TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS, mqttStatusPubTimeUp);
}

void loop() {
#ifdef ON_BOARD_LED
  blink(ON_BOARD_LED);
#endif

  while (!client.connected()) {
    Serial.println("Connection lost, trying to reconnect...");

    if (!connectToMqttServer()) {
      subscribeMqttTopic();
    } else {
      initWifiSta();
    }
  }
  client.loop();

  delay(LOOP_INTERVAL);
}
