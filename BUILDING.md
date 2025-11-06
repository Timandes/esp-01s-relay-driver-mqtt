# ESP8266项目构建和部署指南

本文档描述了如何使用Makefile构建、验证和部署ESP8266项目。

## 先决条件

1. 安装Arduino CLI
2. 安装ESP8266核心支持
3. 安装Make工具
4. 安装所需库（ArduinoJson、PubSubClient）
5. 如果启用PWM功能，还需要安装ESP8266_PWM库

## 安装所需库

在首次构建项目之前，您需要安装所需的库：

```bash
# 安装ArduinoJson库
arduino-cli lib install ArduinoJson

# 安装PubSubClient库
arduino-cli lib install PubSubClient
```

如果启用了PWM功能（在config.h中定义了PWM_ENABLED），还需要安装ESP8266_PWM库：

```bash
# 安装ESP8266_PWM库
arduino-cli lib install ESP8266_PWM
```

## 构建流程

### 1. 使用Makefile构建
```bash
make build
```

构建产物将被放置在项目根目录下的 `build/` 目录中，包括：
- `.bin` 文件 - 二进制固件文件，用于上传到ESP8266
- `.elf` 文件 - 可执行文件，包含调试信息
- `.map` 文件 - 内存映射文件，用于分析内存使用情况

### 2. 直接使用Arduino CLI构建
```bash
arduino-cli compile --fqbn esp8266:esp8266:generic wake-up.ino
```

## 部署流程

### 1. 使用Makefile上传
```bash
# 将/dev/ttyUSB0替换为实际的串行端口
make upload PORT=/dev/ttyUSB0
```

### 2. 直接使用Arduino CLI上传
```bash
# 先编译
arduino-cli compile --fqbn esp8266:esp8266:generic wake-up.ino

# 然后上传（将/dev/ttyUSB0替换为实际的串行端口）
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:generic wake-up.ino
```

## 配置文件

项目使用`config.h`文件进行配置。如果该文件不存在，请从`config.example.h`复制：
```bash
cp config.example.h config.h
```

然后根据需要修改`config.h`中的配置值。

## 可用的Make命令

```bash
make              # 编译项目 (默认目标)
make build        # 编译项目
make upload       # 编译并上传项目到ESP8266 (需要指定PORT参数)
make clean        # 清理编译产物
make help         # 显示帮助信息
```

## 故障排除

1. 如果编译失败，请检查是否正确安装了所需的库（ArduinoJson、PubSubClient）：
   ```bash
   arduino-cli lib list
   ```

   如果启用了PWM功能，还需要确保ESP8266_PWM库已正确安装。

2. 如果上传失败，请检查：
   - ESP8266是否正确连接到计算机
   - 串行端口是否正确指定
   - 是否选择了正确的开发板

3. 查看所有可用的Make命令：
   ```bash
   make help
   ```