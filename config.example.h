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


#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

const char *WIFI_SSID = "";
const char *WIFI_PASSWD = "";

const char *MQTT_HOST = "192.168.1.2";
const uint16_t MQTT_PAYLOAD_BUF_SIZE = 512;

// 命令订阅
const char *MQTT_COMMAND_TOPIC = "hello/esp8266/command";

// 状态发布
uint32_t TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS = 10000;
const char *MQTT_STATUS_TOPIC = "hello/esp8266/status";

const uint8_t CLOSE_STATE = LOW;
const uint8_t INITIAL_STATE = CLOSE_STATE;

const uint8_t ON_BOARD_LED_NORMAL_STATE = LOW;

const uint8_t OUTPUT_PIN = 0;

#define COMPUTER_ON_ENABLED
const unsigned long COMPUTER_ON_INTERVAL = 200;

//#define HOME_ASSISTANT_MQTT_DISCOVER_ENABLED
const char *HOME_ASSISTANT_TOPIC_PREFIX = "homeassistant";
const char *HOME_ASSISTANT_OBJECT_ID = "esp8266";
const char *HOME_ASSISTANT_NAME = "ESP8266";
uint32_t HOME_ASSISTANT_TOPIC_PUBLISH_INTERVAL_IN_MILLIS = 10000;

#define PWM_ENABLED
const char *  PWM_MQTT_COMMAND_TOPIC = "hello/pwm/cmd";
uint32_t      PWM_TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS = 10000;
const char *  PWM_MQTT_STATUS_TOPIC = "hello/pwm/status";
const uint8_t PWM_OUTPUT_PIN = 3;

#endif
