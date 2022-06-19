PORT ?= /dev/ttyACM0
IMAGE_NAME = rubber-nugget
CONTAINER_NAME := $(IMAGE_NAME)-$(shell date +%s)

build:
	docker build . --file Dockerfile --tag rubber-nugget

flash: check_port build
	docker create --name $(CONTAINER_NAME) --device=$(PORT) -t rubber-nugget:latest
	docker start $(CONTAINER_NAME)
	docker exec $(CONTAINER_NAME) bash -c \
		'./arduino-cli upload -b esp32:esp32:esp32s2 --port $(PORT) RubberNugget/ && \
		sleep 2 && \
		python3 -m esptool --after no_reset write_flash 0x110000 scripts.img'
	docker rm --force $(CONTAINER_NAME)

check_port:
	@ls $(PORT) || { echo "Device not found at $(PORT). Run with 'make flash PORT=<port>'" && exit 1; }
