#pragma once

#include "Arduino.h"

class RubberNugget {
  public:
    RubberNugget();
    static void init();
    static void selectPayload(char* path);
  private:
    static void runPayload(char* path);
  
};
