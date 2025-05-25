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

const char *WIFI_SSID = "";
const char *WIFI_PASSWD = "";

const char *MQTT_HOST = "192.168.1.2";
const char *MQTT_TOPIC = "hello/esp8266";

const uint8_t CLOSE_STATE = LOW;

const uint8_t OUTPUT_PIN = 0;

#define COMPUTER_ON_ENABLED
const unsigned long COMPUTER_ON_INTERVAL = 200;

//#define HOME_ASSISTANT_MQTT_DISCOVER_ENABLED
const char *HOME_ASSISTANT_TOPIC_PREFIX = "homeassistant";
const char *HOME_ASSISTANT_OBJECT_ID = "esp8266";
const char *HOME_ASSISTANT_NAME = "ESP8266";
uint32_t HOME_ASSISTANT_TOPIC_PUBLISH_INTERVAL_IN_MILLIS = 10000;

#endif
