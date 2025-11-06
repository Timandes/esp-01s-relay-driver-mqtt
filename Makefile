# Project: wake-up
# MCU: ESP8266
# FQBN: esp8266:esp8266:generic

# 默认目标
.PHONY: all build upload clean help

# 项目名称和文件
PROJECT_NAME = wake-up
INO_FILE = $(PROJECT_NAME).ino
FQBN = esp8266:esp8266:generic
BUILD_DIR = build

# 编译目标
all: build

# 编译项目
build:
	@echo "开始编译项目..."
	arduino-cli compile --fqbn $(FQBN) $(INO_FILE) --output-dir $(BUILD_DIR) && \
	echo "编译成功完成！" && \
	echo "生成的文件在 $(BUILD_DIR) 目录：" && \
	ls -la $(BUILD_DIR) || \
	(echo "编译失败！"; exit 1)


# 上传固件到ESP8266
upload: build
	@if [ -z "$(PORT)" ]; then \
		echo "请通过PORT参数指定串行端口，例如：make upload PORT=/dev/ttyUSB0"; \
		exit 1; \
	fi
	@echo "正在上传固件到 $(PORT)..."
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) --input-dir $(BUILD_DIR) $(INO_FILE) && \
	echo "固件上传成功！" || \
	(echo "固件上传失败！"; exit 1)

# 清理编译产物
clean:
	@echo "清理编译产物..."
	rm -rf $(BUILD_DIR)/*
	@echo "清理完成！"

# 显示帮助信息
help:
	@echo "可用的目标:"
	@echo "  all     - 编译项目 (默认目标)"
	@echo "  build   - 编译项目"
	@echo "  upload  - 编译并上传项目到ESP8266 (需要指定PORT参数)"
	@echo "  clean   - 清理编译产物"
	@echo "  help    - 显示此帮助信息"
	@echo ""
	@echo "使用示例:"
	@echo "  make              # 编译项目"
	@echo "  make build        # 编译项目"
	@echo "  make upload PORT=/dev/ttyUSB0  # 编译并上传到指定端口"
	@echo "  make clean        # 清理编译产物"
	@echo "  make help         # 显示帮助信息"
