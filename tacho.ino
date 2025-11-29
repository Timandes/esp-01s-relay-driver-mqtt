#include<Ticker.h>
#include"tacho.h"

volatile unsigned long tacho_counter = 0;
volatile byte tmp = LOW;
volatile unsigned long tacho_last_time = 0;
unsigned long tacho_last_counter_value = 0;
volatile unsigned long tacho_cur_rpm = 0;

Ticker tacho_display_timer;
Ticker tacho_status_pub_timer;
Ticker tacho_cal_timer;

uint32_t TACHO_DEFAULT_DISPLAY_INTERVAL_IN_MILLIS = 3000;
uint32_t TACHO_DEFAULT_CAL_INTERVAL_IN_MILLIS = 3000;

void IRAM_ATTR tacho_update_counter() {
    tacho_counter++;
    tmp = !tmp;
}

void IRAM_ATTR tacho_update_counter_v2() {
    unsigned long now = micros();
    unsigned long elapsed = now - tacho_last_time;
    unsigned long rpm = (unsigned long)(30 * 1000000 / elapsed);

    if (rpm < 5000) {
        tacho_cur_rpm = rpm;
    }

    tacho_last_time = now;

    tmp = !tmp;
}

void tacho_display_timer_handler() {
    Serial.print(F("Cur. RPM = ")); Serial.println(tacho_get_cur_rpm());
}

void tacho_reset_counter() {
    tacho_counter = 0;
    tacho_last_counter_value = 0;
    tacho_last_time = millis();
}

void tacho_init(uint8_t pin) {
    Serial.print("Initializing TACHO pin ="); Serial.println(pin);
    //pinMode(pin, INPUT_PULLUP);
    pinMode(pin, INPUT);
    tacho_reset_counter();
    // V1
    attachInterrupt(digitalPinToInterrupt(pin), tacho_update_counter, FALLING);
    //attachInterrupt(digitalPinToInterrupt(pin), tacho_update_counter, CHANGE);
    // V2
    //attachInterrupt(digitalPinToInterrupt(pin), tacho_update_counter_v2, FALLING);

    Serial.print("Reporting TACHO value in every "); Serial.print(TACHO_DEFAULT_DISPLAY_INTERVAL_IN_MILLIS); Serial.println(" ms");
    tacho_display_timer.attach_ms(TACHO_DEFAULT_DISPLAY_INTERVAL_IN_MILLIS, tacho_display_timer_handler);

    Serial.print("Reporting TACHO value to MQTT in every "); Serial.print(TACHO_TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS); Serial.println(" ms");
    tacho_status_pub_timer.attach_ms(TACHO_TOPIC_STATUS_PUBLISH_INTERVAL_IN_MILLIS, tacho_mqtt_status_pub_time_up);

    // V1
    tacho_cal_timer.attach_ms(TACHO_DEFAULT_CAL_INTERVAL_IN_MILLIS, tacho_cal_time_up);
}

void tacho_init_home_assistant_device() {
    String topic = HOME_ASSISTANT_TOPIC_PREFIX;
    topic += "/sensor/";
    topic += TACHO_HOME_ASSISTANT_OBJECT_ID;
    topic += "/config";

    Serial.print("Publishing to TACHO Topic: ");
    Serial.println(topic);

    DynamicJsonDocument doc(1024);
    doc["name"] = TACHO_HOME_ASSISTANT_NAME;
    doc["state_topic"] = TACHO_MQTT_STATUS_TOPIC;
    doc["command_topic"] = TACHO_MQTT_STATUS_TOPIC;
    doc["object_id"] = TACHO_HOME_ASSISTANT_OBJECT_ID;
    doc["unique_id"] = TACHO_HOME_ASSISTANT_OBJECT_ID;
    doc["device"]["identifiers"][0] = HOME_ASSISTANT_DEVICE_IDENTIFIER;
    doc["device"]["name"] = HOME_ASSISTANT_DEVICE_NAME;
    doc["device"]["manufacturer"] = "Timandes White";
    doc["unit_of_measurement"] = "rpm";
    doc["device_class"] = "speed";


    String payload = "";
    serializeJson(doc, payload);
    boolean r = client.publish_P(topic.c_str(), payload.c_str(), false);
    if (r) {
        Serial.print("TACHO Message published: ");
    } else {
        Serial.print("TACHO Message publishing failed: ");
    }
    Serial.println(payload);
    Serial.print("payload.len=");
    Serial.println(payload.length());
}

void tacho_loop() {
    digitalWrite(LED_BUILTIN, tmp);
}

unsigned long tacho_get_cur_rpm() {
    return tacho_cur_rpm;
}

void tacho_cal_cur_rpm() {
    unsigned long counter = tacho_counter;
    unsigned long acc = counter - tacho_last_counter_value;
    tacho_last_counter_value = counter;

    unsigned long now = millis();
    unsigned long elapsed = now - tacho_last_time;

    if (elapsed > 0) {
        //tacho_cur_rpm = (unsigned long)((acc * 30) / (elapsed / 1000));
        tacho_cur_rpm = (unsigned long)((acc * 30 * 1000) / elapsed);
    } else {
        tacho_cur_rpm = 0;
    }

    tacho_reset_counter();
/*
    if (counter > 2294967295) {
        tacho_reset_counter();
    }*/
}

void tacho_cal_time_up() {
    Serial.println("MQTT TACHO Cal. time up");
    tacho_cal_cur_rpm();
}

void tacho_pub_status() {
    Serial.print("Publishing to TACHO Topic: ");
    Serial.println(TACHO_MQTT_STATUS_TOPIC);

    String payload = String(tacho_get_cur_rpm());
    boolean r = client.publish_P(TACHO_MQTT_STATUS_TOPIC, payload.c_str(), false);
    if (r) {
        Serial.print("TACHO Message published: ");
    } else {
        Serial.print("TACHO Message publishing failed: ");
    }
    Serial.println(payload);
    Serial.print("payload.len=");
    Serial.println(payload.length());
}

void tacho_mqtt_status_pub_time_up() {
    Serial.println("MQTT TACHO Status Pub time up");
    tacho_pub_status();
}
