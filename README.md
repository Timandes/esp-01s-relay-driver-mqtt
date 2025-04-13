# ESP-01S 继电器智能家居驱动



主要解决电脑远程开关机等远程控制问题。使用到的硬件模块包括：

* ESP-01S
* 与其配套的继电器模块
* 其他公公、公母杜邦线和连接夹等

使用到的软件、服务和项目包括：

* MQTT
* [PubSubClient??](https://github.com/knolleary/pubsubclient)
* Home Assistant（可选）



## 基本原理

驱动启动后，立即尝试进行WiFi连接。在成功连入WiFi网络后，就会尝试向MQTT服务器发起连接，随后完成向Topic的订阅。

当有人向对应的Topic投递消息后，如果消息匹配，则驱动会通过IO0连续发送`!CLOSE_STATE`和`CLOSE_STATE`状态，间隔200ms。如果配置正确的话，就会触发继电器常闭端（NC）的打开和关闭，实现电脑远程开机。



## 马上开始

1. 复制`config.example.h`到`config.h`；

2. 修改`config.h`中的WiFi、MQTT服务器及继电器处于关闭状态时的状态值（HIGH|LOW）；

3. 编译并完成烧录；

4. 将ESP-01S插回继电器模块，完成电路连线并通电；

5. 通过向MQTT队列主题发送`ComputerOn`字符串以便完成开机；



## 安装步骤

### MQTT

可以安装任意兼容MQTT协议的队列服务器。推荐使用Docker容器安装EMQX。具体参看文档：

[Download EMQX Open Source](https://www.emqx.com/en/downloads-and-install/broker)



初次安装的新手建议同步安装支持MQTT协议的客户端工具MQTTX，以便手工测试MQTT服务器的状态，以及进行模拟请求。具体参看文档：

[MQTTX 下载](https://mqttx.app/zh/downloads)



### PubSubClient

### Home Assistant（可选）



## 接线图



## 配置说明

1. WiFi配置

**WIFI_SSID**：连接WiFi的SSID。一般是WiFi的名字。

**WIFI_PASSWD**：连接WiFi的密码。



2. MQTT服务器配置

**MQTT_HOST**：MQTT服务器的地址。也可以包含用冒号分割的端口号，如：`192.168.1.2:1234`。

**MQTT_TOPIC**：MQTT订阅主题名字。



3. 继电器配置

**CLOSE_STATE**：继电器处于关闭状态时应该给出的电平状态。可以给出LOW或者HIGHT，默认是LOW（低电平）。

**OUTPUT_PIN**：继电器信号输出管脚。默认为GPIO0（0值）。

**COMPUTER_ON_INTERVAL**：ComputerOn的间隔，单位ms（默认200ms）。



## MQTT消息

目前支持的消息如下表所示：

| 消息字符串 | 动作                                                        |
| ---------- | ----------------------------------------------------------- |
| ON         | IO0高电平（HIGH）                                           |
| OFF        | IO0低电平（LOW）                                            |
| ComputerOn | 通过IO0连续发送`!CLOSE_STATE`和`CLOSE_STATE`状态，间隔200ms |



## 继电器测试方法

ESP-01S配套继电器有8孔插座。如果按照ESP-01S的插针在上方视角来看：

* 左上角是3.3V
* 右下角是GND
* 第二排第二个插针是IO0



在正确接入5V（或3.3V，根据继电器不同）后，首先取一根公公杜邦线插入左上角3.3V插孔，然后将另一端插入IO0插孔，如果继电器打开（LED灯也会随之点亮），则`CLOSE_STATE`可以填LOW；如果一段插入右下角GND，另一端插入IO0插孔时继电器打开，则`CLOSE_STATE`可以填HIGH。

请注意后一种情况的继电器模块可能会在插入ESP-01S通电时NC端会出现闪开、闪关。这时如果已经连接了电脑Power Switch，可能会触发开机；同理，当电脑完全断电后突然通电时，也会立即开机。这种继电器模块也是可以使用的，只要保证一直通电就行了。如果有能力调整继电器的线路，可以将原GPIO0的线路割断，同时将RXD（GPIO3）接入原线路；然后修改`OUTPUT_PIN`值为3，就可以完美解决上述问题。

如果上述任何情况都不能打开使继电器打开，就有可能是继电器或其电路故障。建议更换继电器模块。
