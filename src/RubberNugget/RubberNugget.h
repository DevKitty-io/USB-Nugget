#pragma once

#include "Arduino.h"
#include "cdcusb.h"
#include "mscusb.h"
#include "flashdisk.h"

#include <Adafruit_NeoPixel.h>
extern Adafruit_NeoPixel strip;

class RubberNugget {
  public:
    RubberNugget();
    static void init();
    static void selectPayload(char* path);
    static void runPayload(char* path, uint8_t from);
    static void runLivePayload(String payloadRaw);
    static String getPayloads();
  
};
