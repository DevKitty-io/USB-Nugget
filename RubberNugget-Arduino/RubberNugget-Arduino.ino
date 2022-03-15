#include "RubberNugget.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels {1, 12, NEO_GRB + NEO_KHZ800 };



void setup() {
    Serial.println(115200);

    // initialize & launch payload selector
    
    RubberNugget::init();
    RubberNugget::selectPayload("/");  
    
}

void loop() {
  return;
}
