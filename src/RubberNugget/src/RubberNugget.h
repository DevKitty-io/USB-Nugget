#pragma once

#include "Arduino.h"
#include "cdcusb.h"
#include "flashdisk.h"
#include "mscusb.h"

class RubberNugget {
 public:
  RubberNugget(){};
  static void init();
  static String* allPayloadPaths(const char* path = "/");
};

FILINFO* newFileList(const char* path, int& numFiles);