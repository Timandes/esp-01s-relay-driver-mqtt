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

// WiFi配置声明
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWD;

// MQTT配置声明
extern const char *MQTT_HOST;
extern const uint16_t MQTT_PAYLOAD_BUF_SIZE;

// 命令订阅声明
extern const char *MQTT_COMMAND_TOPIC;

// 状态发布声明
extern uint32_t TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS;
extern const char *MQTT_STATUS_TOPIC;

// 状态配置声明
extern const uint8_t CLOSE_STATE;
extern const uint8_t INITIAL_STATE;
extern const uint8_t ON_BOARD_LED_NORMAL_STATE;
extern const uint8_t OUTPUT_PIN;

// 计算机开机配置声明
extern const unsigned long COMPUTER_ON_INTERVAL;

// Home Assistant配置声明
extern const char *HOME_ASSISTANT_TOPIC_PREFIX;
extern const char *HOME_ASSISTANT_OBJECT_ID;
extern const char *HOME_ASSISTANT_NAME;
extern uint32_t HOME_ASSISTANT_TOPIC_PUBLISH_INTERVAL_IN_MILLIS;

// PWM功能配置声明
extern const char*   PWM_MQTT_COMMAND_TOPIC;
extern uint32_t      PWM_TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS;
extern const char*   PWM_MQTT_STATUS_TOPIC;
extern const uint8_t PWM_OUTPUT_PIN;
extern const char *PWM_HOME_ASSISTANT_OBJECT_ID;
extern const char *PWM_HOME_ASSISTANT_NAME;

#endif