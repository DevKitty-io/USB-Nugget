PORT ?= /dev/ttyACM0
RESET_SCRIPTS_DURING_FLASH ?= false
IMAGE_NAME = rubber-nugget
CONTAINER_NAME := $(IMAGE_NAME)-$(shell date +%s)

submodules:
	git submodule init
	git submodule update

build: submodules
	docker build . --file Dockerfile --tag rubber-nugget

flash: check_port build
	docker create --name $(CONTAINER_NAME) --device=$(PORT) -t rubber-nugget:latest
	docker start $(CONTAINER_NAME)
	docker exec $(CONTAINER_NAME) bash -c \
		'./arduino-cli upload -b esp32:esp32:esp32s2 --port $(PORT) RubberNugget/ && sleep 2'
ifeq ($(RESET_SCRIPTS_DURING_FLASH), true)
	docker exec $(CONTAINER_NAME) bash -c \
		'python3 -m esptool --after no_reset write_flash 0x110000 default.img'
endif
	docker rm --force $(CONTAINER_NAME)

generate_img: check_port build
	docker create --name $(CONTAINER_NAME) --device=$(PORT) -t rubber-nugget:latest
	docker start $(CONTAINER_NAME)
	docker exec $(CONTAINER_NAME) bash -c \
		'python3 -m esptool --after no_reset erase_flash && sleep 2 && \
		./arduino-cli upload -b esp32:esp32:esp32s2 --port $(PORT) RubberNugget/ && sleep 2 && \
		python3 -m esptool --after no_reset write_flash 0x110000 default.img && sleep 2 && \
		python3 -m esptool --after no_reset read_flash 0x0 0x400000 /rubber_nugget.bin'
	docker cp $(CONTAINER_NAME):/rubber_nugget.bin /tmp/meow.bin
	docker rm --force $(CONTAINER_NAME)

check_port:
	@ls $(PORT) || { echo "Device not found at $(PORT). Run with 'make flash PORT=<port>'" && exit 1; }
