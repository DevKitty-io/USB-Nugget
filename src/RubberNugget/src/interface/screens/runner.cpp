#include "runner.h"
#include "../../RubberNugget.h"
#include "../graphics.h"

ScriptRunnerScreen::ScriptRunnerScreen(String path, unsigned long TTL) {
  this->has_run = false;
  this->path = path;
  //this->alwaysUpdates(true);
}

int ScriptRunnerScreen::update(int btn) {
  if (!has_run) {
    RubberNugget::runPayload(this->path.c_str());
    has_run = true;
    return SCREEN_REDRAW;
  }
  if (btn==BTN_LEFT){
    return SCREEN_BACK;
  }
  return SCREEN_REDRAW;
}

bool ScriptRunnerScreen::draw() {
  if (!has_run) {
    return false;
  }
  display->drawXbm(0, 0, 128, 64, high_signal_bits);
  display->drawString(3,9,"Press LEFT");
  display->drawString(3,19,"to go back");
  display->drawLine(0, 54, 127, 54);
  display->drawLine(0, 53, 127, 53);
  display->drawString(0, 54, "FINISHED PAYLOAD");

  //display->drawString(3,12, this->path);
  //display->drawXbm(0, 0, 128, 64, RubberNugget_bits);
  //return true;
  return true;
}
