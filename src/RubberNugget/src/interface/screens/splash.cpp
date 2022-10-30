#include "splash.h"
#include "../graphics.h"

SplashScreen::SplashScreen(unsigned long durationInMs) {
    this->endAt = millis()+durationInMs;
}

int SplashScreen::update(int button) {
    // update even when there's no keypress
    this->alwaysUpdates(true);
    unsigned int currentTime = millis();
    Serial.printf("Current time: %lu, end at: %lu", currentTime, this->endAt);
    if (currentTime > this->endAt) {
        return SCREEN_BACK;
    }
    if (button==EVENT_INIT){
        return SCREEN_REDRAW;
    }
    return SCREEN_NONE;
}

bool SplashScreen::draw() {
  display->drawXbm(0, 0, 128, 64, splash_bits);
  display->drawString(100,0,"v1.2");
  display->drawRect(98,0,30,12);
  return true;
}
