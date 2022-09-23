#ifndef SCRIPT_RUNNER_SCREEN_H
#define SCRIPT_RUNNER_SCREEN_H

#include "../lib/NuggetInterface.h"


class ScriptRunnerScreen : public NuggetScreen {
  public:
    ScriptRunnerScreen(String path, unsigned long TTL);
    bool draw();
    int update(int);
  private:
    String path;
    bool has_run;
};

#endif
