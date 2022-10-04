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
    static void selectPayload();
    static String* allPayloadPaths(const char* path="/");

  private:
    static void setDefaults();
};

FILINFO* newFileList(const char* path, int& numFiles);