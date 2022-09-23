#include "dir.h"

// newFileList2 returns a paginated list of strings representing the files
// available at the given path. resultsPerPage will be set to number of results
// returned. On error, it returns nullptr and numFiles is set to -1.
FILINFO* newFileList2(const char* path, int& numFiles) {
  uint currentCapacity = 4; // arbitrary, will grow as needed
  uint filesSoFar = 0;
  bool errored = false;
  FILINFO* fileList = new FILINFO[currentCapacity];

  FRESULT res;
  FF_DIR dir;
  FILINFO fno;

  res = f_opendir(&dir, path);
  if (res != FR_OK) {
    Serial.printf("[newFileList] f_opendir error: %d\n", res);
    goto onError;
  }

  for (;;) {
    res = f_readdir(&dir, &fno); // Read a directory item
    if (res != FR_OK) {
      errored = true;
      break;
    }
    if (fno.fname[0] == 0) { // End of dir
      break;
    }
    if (fno.fname[0] == '.') { // Don't show files starting with '.'
      continue;
    }
    // Found a file/dir. Store it, doubling capacity first if needed.
    if (filesSoFar >= currentCapacity) {
      FILINFO* newFileList = new FILINFO[currentCapacity*2];
      for (int i = 0; i < currentCapacity; i++){
        newFileList[i] = fileList[i];
      }
      currentCapacity *= 2;
      delete[] fileList;
      fileList = newFileList;
    }
    fileList[filesSoFar] = fno;
    filesSoFar++;
  }
  if (errored) {
    Serial.printf("[newFileList] f_readdir error: %d\n", res);
    goto onError;
  }
  res = f_closedir(&dir);
  if (res != FR_OK) {
    Serial.printf("[newFileList] f_closedir error: %d\n", res);
    goto onError;
  }
  if (filesSoFar < 1){
    Serial.println("[newFileList] directory empty");
    goto onError;
  }
  numFiles = filesSoFar;
  return fileList;

onError:
  numFiles = -1;
  delete[] fileList;
  return nullptr;
}

DirScreen::DirScreen(String path) {
  this->path = path;
  this->files = nullptr;
  this->numFiles = 0;
  this->selected = 0;
  this->top = 0;

  // TODO: files could be null; draw/update should handle this
  this->files = newFileList2(path.c_str(), numFiles);
}

DirScreen::~DirScreen() {
  if (files){
    delete this->files;
  }
}

int DirScreen::update(int btn) {
  switch(btn){
    case BTN_UP:
      if (this->selected != 0) {
          this->selected--;
      }
      if (this->selected < this->top) {
          this->top--;
      }
      break;
    case BTN_DOWN:
      if (this->selected < this->numFiles-1) {
          this->selected++;
      }
      if (this->selected-this->top > DIR_SCREEN_MAX_SHOWN-1){
          this->top++;
      }
      break;
    case BTN_LEFT:
      return SCREEN_BACK;
    case BTN_RIGHT:
      if(files[selected].fattrib & AM_DIR){ // directory; enter it
        String nextPath(path);
        if (nextPath.length()!=1){
          nextPath += "/";
        }
        nextPath += files[selected].fname;
        Serial.printf("Before dir screen alloc. Mem: %d\n", ESP.getFreeHeap());
        DirScreen* subdir = new DirScreen(nextPath);
        this->pushScreen(subdir);
        return SCREEN_PUSH;
      } else { // file; assume it's a payload and run it
        String payload = path;
        if (payload.length()!=1){
          payload += "/";
        }
        payload += files[selected].fname;
        Serial.printf("Before runner alloc. Mem: %d\n", ESP.getFreeHeap());
        ScriptRunnerScreen* runner = new ScriptRunnerScreen(payload, 2000);
        this->pushScreen(runner);
        return SCREEN_PUSH;
        //runPayload(payload.c_str(), 0);
      }
  }
  return SCREEN_REDRAW;
}


//     item0             <-- files; item0 extends "above" screen
// ┌──────────────────┐
// │   item1          │  <-- top=1
// │   item2          │
// │ > item3          │  <-- cursor=3; third item, not third from top
// │   item4          │  <-- numFiles=5
// ├──────────────────┤
// │/path/items       │  <-- footer
// └──────────────────┘
bool DirScreen::draw() {
  if (!files || this->selected >= this->numFiles || (this->top > 0 && this->numFiles < DIR_SCREEN_MAX_SHOWN)) {
    Serial.printf("[displayFiles] condition error: (files:%p)(numFiles:%d)(selected:%d)(top:%d)\n");
    // TODO: display error screen
    return false;
  }

  // loop condition: display only DIR_SCREEN_MAX_SHOWN at a time, and not more
  // than files we actually have.
  for (int i = top; i < top+DIR_SCREEN_MAX_SHOWN && i < numFiles; i++) {
    String fileName(files[i].fname);
    if (fileName.length() > 12) {
      fileName = fileName.substring(0, 9) + "...";
    }
    display->drawString(6, 10*(i-top), fileName);
  }
  // Cursor
  display->drawRect(2,10*(selected-top)+6,2,2);

  // Footer
  display->drawLine(0, 54, 127, 54);
  display->drawLine(0, 53, 127, 53);
  display->drawString(0,54,"Dir:");
  if(path.length() > 17) {
    display->drawString(25, 54, path.substring(0,14)+"...");
  } else {
    display->drawString(25, 54, path);
  }
  // Cat image
  //display->drawXbm(0, 0, 128, 64, RubberNugget_bits);
  display->drawString(100,0,"v1.1");
  display->drawRect(98,0,30,12);

  return true;
}

