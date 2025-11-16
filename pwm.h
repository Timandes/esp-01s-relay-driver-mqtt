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


#ifndef PWM_H
#define PWM_H

#include <ESP8266_PWM.h>

// PWM相关全局变量声明
extern volatile uint32_t pwm_start_micro;
extern ESP8266Timer pwm_timer;
extern ESP8266_PWM pwm_control;
extern float pwm_current_value;
extern int pwm_channel_no;
extern Ticker pwm_status_pub_timer;

// PWM函数声明
void IRAM_ATTR pwm_timer_handler();
void pwm_init();
void pwm_set(float value);
String pwm_read_status_string();
void pwm_pub_status();
void pwm_mqtt_status_pub_time_up();
void pwm_init_home_assistant_device();

#endif
