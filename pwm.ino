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


#ifndef PWM_HPP
#define PWM_HPP

#include <ESP8266_PWM.h>
#include "pwm.h"
#include "config.hpp"

// PWM相关全局变量定义
volatile uint32_t pwm_start_micro = 0;
ESP8266Timer pwm_timer;
ESP8266_PWM pwm_control;
float pwm_current_value = 0;
int pwm_channel_no = -1;
Ticker pwm_status_pub_timer;

// PWM函数实现
void IRAM_ATTR pwm_timer_handler()
{
    pwm_control.run();
}

void pwm_init() {
    // Interval in microsecs
    if (pwm_timer.attachInterruptInterval(20L, pwm_timer_handler))  // 使用字面值而不是常量
    {
        pwm_start_micro = micros();
        Serial.print(F("Starting pwm_timer OK, micros() = ")); Serial.println(pwm_start_micro);
    }
    else {
        Serial.println(F("Can't set pwm_timer. Select another freq. or timer"));
    }

    pwm_set(45.0);
}

void pwm_set(float value) {
    if (pwm_channel_no >= 0) {
        pwm_control.deleteChannel((uint8_t) pwm_channel_no);
    }
    uint32_t freq = 500;
    // You can use this with PWM_Freq in Hz
    pwm_channel_no = pwm_control.setPWM(PWM_OUTPUT_PIN, freq, value);
    if (pwm_channel_no < 0) {
        extern unsigned long onBoardLightBlinkInterval;
        onBoardLightBlinkInterval = 30;
    } else {
        extern unsigned long onBoardLightBlinkInterval;
        onBoardLightBlinkInterval = 0;
    }
    pwm_current_value = value;

    pwm_pub_status();
}

String pwm_read_status_string() {
    return String(pwm_current_value);
}

void pwm_pub_status() {
    extern PubSubClient client;
    Serial.print("Publishing to PWM Topic: ");
    Serial.println(PWM_MQTT_STATUS_TOPIC);

    String payload = pwm_read_status_string();
    boolean r = client.publish_P(PWM_MQTT_STATUS_TOPIC, payload.c_str(), false);
    if (r) {
        Serial.print("PWM Message published: ");
    } else {
        Serial.print("PWM Message publishing failed: ");
    }
    Serial.println(payload);
    Serial.print("payload.len=");
    Serial.println(payload.length());
}

void pwm_mqtt_status_pub_time_up() {
    Serial.println("MQTT PWM Status Pub time up");
    pwm_pub_status();
}

#ifdef HOME_ASSISTANT_MQTT_DISCOVER_ENABLED
void pwm_init_home_assistant_device() {
    extern PubSubClient client;
    String topic = HOME_ASSISTANT_TOPIC_PREFIX;
    topic += "/number/";
    topic += PWM_HOME_ASSISTANT_OBJECT_ID;
    topic += "/config";

    Serial.print("Publishing to PWM Topic: ");
    Serial.println(topic);

    DynamicJsonDocument doc(1024);
    doc["name"] = serialized("null");
    doc["state_topic"] = PWM_MQTT_STATUS_TOPIC;
    doc["command_topic"] = PWM_MQTT_COMMAND_TOPIC;
    doc["object_id"] = PWM_HOME_ASSISTANT_OBJECT_ID;
    doc["unique_id"] = PWM_HOME_ASSISTANT_OBJECT_ID;
    doc["device"]["identifiers"][0] = PWM_HOME_ASSISTANT_OBJECT_ID;
    doc["device"]["name"] = PWM_HOME_ASSISTANT_NAME;
    doc["min"] = 0.0;
    doc["max"] = 100.0;
    doc["step"] = 1;
    doc["mode"] = "slider";

    String payload = "";
    serializeJson(doc, payload);
    boolean r = client.publish_P(topic.c_str(), payload.c_str(), false);
    if (r) {
        Serial.print("PWM Message published: ");
    } else {
        Serial.print("PWM Message publishing failed: ");
    }
    Serial.println(payload);
    Serial.print("payload.len=");
    Serial.println(payload.length());
}
#endif

#endif
