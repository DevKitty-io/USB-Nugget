# syntax=docker/dockerfile:1
FROM debian:buster
WORKDIR /app

RUN apt-get update && apt-get install -y \
    wget \
    zip \
    bsdmainutils \
    xz-utils \
    python3 python3-pip
RUN pip3 install pyserial esptool

ARG ARDUINO_CLI_VERSION=0.22.0
RUN wget https://github.com/arduino/arduino-cli/releases/download/${ARDUINO_CLI_VERSION}/arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz
RUN tar -xf arduino-cli_${ARDUINO_CLI_VERSION}_Linux_64bit.tar.gz

# TODO: version-lock these
COPY arduino-cli.yaml .
RUN ./arduino-cli core update-index --config-file arduino-cli.yaml
RUN ./arduino-cli core install esp32:esp32
RUN ./arduino-cli lib install \
    "ESP8266 and ESP32 OLED driver for SSD1306 displays" \
    "Adafruit NeoPixel"
COPY third_party ./third_party
RUN ./third_party/arduino_cli_install.sh $(readlink -f ./arduino-cli)

# Patch to force esptool to use no_reset option. This is necessary because
# arduino-cli does not provide an option to forward this flag. This may become
# unncessary when esp32s2 is better supported.
RUN sed -i '/.*args = parser.parse_args(argv)/a\ \ \ \ args.after="no_reset"' /root/.arduino15/packages/esp32/tools/esptool_py/3.3.0/esptool.py

COPY RubberNugget ./RubberNugget
COPY fatfs/default.img .
COPY scripts ./scripts
RUN ./scripts/build_web_ui.sh
RUN ./arduino-cli compile -b esp32:esp32:esp32s2 RubberNugget \
    --build-property build.partitions=noota_3gffat \
    --build-property build.cdc_on_boot=1
