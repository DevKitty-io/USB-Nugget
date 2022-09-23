#ifndef NUGGET_INTERFACE_H
#define NUGGET_INTERFACE_H

#include "SH1106Wire.h"

//----------------------------------------
// NuggetInputs

#define BTN_NONE   -1
#define BTN_PRESS   0
#define BTN_NPRESS  1

#define BTN_COUNT  4
#define BTN_UP     9
#define BTN_DOWN  18
#define BTN_LEFT  11
#define BTN_RIGHT  7

#define EVENT_INIT 100

class NuggetScreen;

class NuggetInputs {
   public:
      NuggetInputs();
      int getInput();
   private:
      void addButton(int);
      int buttons[BTN_COUNT];
      int pressedButton;
      int lastBtn;
};

//----------------------------------------
// NuggetScreen

class NuggetInterface;

#define SCREEN_NONE   1
#define SCREEN_BACK   2
#define SCREEN_REDRAW 3
#define SCREEN_PUSH   4

class NuggetScreen {
   public:
      NuggetScreen();
      virtual bool draw() = 0;
      virtual int update(int){return SCREEN_NONE;};
      void setDisplay(SH1106Wire*);
      void setInputs(NuggetInputs*);
      void setNuggetInterface(NuggetInterface*);
      int _update();
   protected:
      SH1106Wire* display;
      NuggetInputs* inputs;
      void pushScreen(NuggetScreen*);
      void alwaysUpdates(bool);
   private:
      NuggetInterface* nuggetInterface;
      bool alwaysUpdate;
};

//----------------------------------------
// ScreenNode

struct ScreenNode {
   NuggetScreen* screen;
   ScreenNode* prev;
};


//----------------------------------------
// NuggetInterface

class NuggetInterface {
  public:
    NuggetInterface();
    ~NuggetInterface();
    bool start();
    bool draw();
    bool pushScreen(NuggetScreen*);
    bool popScreen();
  private:
    SH1106Wire* display;
    NuggetInputs* inputs;
    ScreenNode* currentScreenNode;
};


#endif

