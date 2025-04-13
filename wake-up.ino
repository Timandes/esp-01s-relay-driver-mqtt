/*
   Copyright 2025 Timandes White

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */
 

#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include<Ticker.h>
#include"config.h"

#define GPIO OUTPUT_PIN

#ifdef D9
#define ON_BOARD_LED D9
#endif
#ifdef LED_BUILTIN
#define ON_BOARD_LED LED_BUILTIN
#endif

#define LOOP_INTERVAL 10

#define OFF_STATE CLOSE_STATE
#define ON_STATE !OFF_STATE

WiFiClient wiFiClient;
PubSubClient client(wiFiClient);

Ticker ticker;

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

void switchOnComputer() {
  Serial.print("Switching on computer(");
  Serial.print(ON_STATE);
  Serial.print(" -> ");
  Serial.print(OFF_STATE);
  Serial.print(")...");

  digitalWrite(GPIO, ON_STATE);
  delay(200);
  digitalWrite(GPIO, OFF_STATE);

  Serial.println("DONE");
}

void mqttMessageArrived(char *topic, byte *payload, unsigned int payloadLen) {
  String message = toString(payload, payloadLen);
  Serial.print("Got message: ");
  Serial.println(message);

  if (message == "ON" || message == "on") {
    digitalWrite(GPIO, ON_STATE);
    Serial.println("GPIO HIGH");
  } else if (message == "OFF" || message == "off") {
    digitalWrite(GPIO, OFF_STATE);
    Serial.println("GPIO LOW");
  } else if (message == "ComputerOn") {
    switchOnComputer();
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
  Serial.println(MQTT_TOPIC);

  if (client.subscribe(MQTT_TOPIC)) {
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


void setup() {
  Serial.begin(115200);
  Serial.println("");

#ifdef ON_BOARD_LED
  initLed(ON_BOARD_LED);
#endif
  initGpio();
  initWifiSta();
  initMqtt();
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
