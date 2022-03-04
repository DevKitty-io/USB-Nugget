#include "RubberNugget.h"
#include "Arduino.h"


void setup() {
    Serial.println(115200);

    // initialize & launch payload selector
    
    RubberNugget::init();
    RubberNugget::selectPayload("/");  
    
}

void loop() {
  return;
}
