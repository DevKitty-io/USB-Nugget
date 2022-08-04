#define up_btn 9 // left button
#define dn_btn 18 // right button
#define lt_btn 11 // left button
#define rt_btn 7 // right button

#define MAX_FILE_SELECTIONS 4
    
#include "RubberNugget.h"
#include "Arduino.h"

#include "Nugget_Interface.h" // construct Nugget Interface
#include "Nugget_Buttons.h"

#include "SH1106Wire.h"
#include "graphics.h"
#include "keyboardlayout.h"

SH1106Wire display(0x3C, 33, 35);

Nugget_Buttons nuggButtons(up_btn,dn_btn,lt_btn,rt_btn);

extern bool webstuffhappening;

// ESPTinyUSB libraries
#include "cdcusb.h"
#include "mscusb.h"
#include "flashdisk.h"
#include "hidkeyboard.h"

HIDkeyboard keyboard;
CDCusb CDCUSBSerial;
FlashUSB fat1;

char *l1 = "ffat";
String payloadPath = "";

/*-----------------------------------------------------------------*/

class MyCDCCallbacks : public CDCCallbacks {
    void onCodingChange(cdc_line_coding_t const* p_line_coding)
    {
        int bitrate = CDCUSBSerial.getBitrate();
        Serial.printf("new bitrate: %d\n", bitrate);
    }

    bool onConnect(bool dtr, bool rts)
    {
        Serial.printf("connection state changed, dtr: %d, rts: %d\n", dtr, rts);
        return true;  // allow to persist reset, when Arduino IDE is trying to enter bootloader mode
    }

    void onData()
    {
        int len = CDCUSBSerial.available();
        Serial.printf("\nnew data, len %d\n", len);
        uint8_t buf[len] = {};
        CDCUSBSerial.read(buf, len);
        Serial.write(buf, len);
    }
};

class Device: public USBCallbacks {
    void onMount() { Serial.println("Mount"); }
    void onUnmount() { Serial.println("Unmount"); }
    void onSuspend(bool remote_wakeup_en) { Serial.println("Suspend"); }
    void onResume() { Serial.println("Resume"); }
};

class MyHIDCallbacks : public HIDCallbacks
{
    void onData(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
    {
        Serial.printf("ID: %d, type: %d, size: %d\n", report_id, (int)report_type, bufsize);
        for (size_t i = 0; i < bufsize; i++)
        {
            Serial.printf("%d\n", buffer[i]);
        }
    }
};

void echo_all(char c) {
    CDCUSBSerial.write(c);
    Serial.write(c);
}

/*-----------------------------------------------------------------*/

Nugget_Interface payloadSelector;
Nugget_Interface payloadRun;
  
RubberNugget::RubberNugget() {
  
}

void RubberNugget::init() {
    
    
    
    display.init();
    display.flipScreenVertically();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(DejaVu_Sans_Mono_10);

    keyboard.setBaseEP(3);
    keyboard.begin();
    keyboard.setCallbacks(new MyHIDCallbacks());

  // mount FAT fs
  if (fat1.init("/fat1", "ffat")) {
    
        //disable this on startup
        if (fat1.begin()) {
            Serial.println("MSC lun 1 begin");
        }
        else {
            log_e("LUN 1 failed");
        }
    }

    if (!CDCUSBSerial.begin())
        Serial.println("Failed to start CDC USB stack");

    CDCUSBSerial.setCallbacks(new MyCDCCallbacks());
    EspTinyUSB::registerDeviceCallbacks(new Device());
}

bool keyKnown(String keyPress) {
  Serial.print("looking for: ");
  Serial.println(keyPress);
  for (int i=0; i< (sizeof(keyMapRN)/sizeof(keyMapRN[0])); i++) {
    if (keyPress.equals(keyMapRN[i].title)) {
      Serial.print(keyMapRN[i].title);
      Serial.println(" found!");
      return true;
    }
  }
  return false;
}

void pressNamedKey(String keyPress, uint8_t modifiers) {
  for (int i=0; i< (sizeof(keyMapRN)/sizeof(keyMapRN[0])); i++) {
    if (keyPress.equals(keyMapRN[i].title)) {
      keyboard.sendPress(keyMapRN[i].key, modifiers);
    }
  }
}

uint16_t defaultDelay = 10;

void processDuckyScript(String ducky) {
  String tCommand = ducky.substring(0, ducky.indexOf(' ')); // get command
  tCommand.toUpperCase(); tCommand.trim();

  ::display.clear();
  payloadRun.addFooter("RUNNING PAYLOAD");
  payloadRun.updateDisplay();
  
  if (tCommand.equals("REM")) {
    Serial.println("Comment");
  }
  else if (tCommand.equals("DELAY")) {
    ::display.drawString(3,12,"DELAY: ");
    ::display.drawString(3,22,(String) ducky.substring(ducky.indexOf(' ')+1, ducky.length()));
    ::display.drawXbm(0, 0, 128, 64, reload_bits);
    payloadRun.updateDisplay();
    delay(ducky.substring(ducky.indexOf(' ')+1, ducky.length()).toInt()); // delay in MS
    Serial.println("Delayed!");       
  }
  else if (tCommand.equals("DEFAULT_DELAY") or tCommand.equals("DEFAULTDELAY")) {
    ::display.drawString(3,12,"DEFAULT");
    ::display.drawString(3,22,"DELAY:");
    ::display.drawString(3,32,(String) ducky.substring(ducky.indexOf(' ')+1, ducky.length()));
    ::display.drawXbm(0, 0, 128, 64, reload_bits);
    payloadRun.updateDisplay();
    defaultDelay = ducky.substring(ducky.indexOf(' ')+1, ducky.length()).toInt();
  }
  else if (tCommand.equals("LED")) {
    ::display.drawString(3,12,"COLOR:");
    ::display.drawString(3,22,(String) ducky.substring(ducky.indexOf(' ')+1, ducky.length())); // accept single color parameter
    ::display.drawXbm(0, 0, 128, 64, reload_bits);
    payloadRun.updateDisplay();
    String color = (String) ducky.substring(ducky.indexOf(' ')+1, ducky.length());
    color.toUpperCase();
    
    if (color.equals("R")) { strip.setPixelColor(0, strip.Color(255,0, 0)); }
    else if (color.equals("G")) {
      strip.setPixelColor(0, strip.Color(0,255, 0));
    }
     else if (color.equals("B")) {
      strip.setPixelColor(0, strip.Color(0,0, 255));
    }
     else if (color.equals("Y")) {
      strip.setPixelColor(0, strip.Color(255,255, 0));
    }
     else if (color.equals("C")) {
      strip.setPixelColor(0, strip.Color(0,255, 255));
    }
     else if (color.equals("M")) {
      strip.setPixelColor(0, strip.Color(255,0, 255));
    }
     else if (color.equals("W")) {
      strip.setPixelColor(0, strip.Color(120,120, 120));
    }
    strip.show(); strip.show();
  }
  else if (tCommand.equals("STRING")) {
    ::display.drawString(3,12,"STRING: ");
    if (String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())).length() > 11) {
      ::display.drawString(3,22,String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())).substring(0,8)+"...");
    }
    else {
      ::display.drawString(3,22,String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())));
    }
    display.drawXbm(0, 0, 128, 64, medium_signal_bits);
    payloadRun.updateDisplay();
    Serial.println("String");
    keyboard.sendString(String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())));    
  }  
  
  else if (keyKnown(tCommand)) {
    ::display.drawString(3,12,"KEY PRESS");
    ducky.trim(); // remove leading, trailing whitespace
    int currentTokenLeftIndex = 0;
    int currentTokenRightIndex = 0;
    String currentToken;
    uint8_t modifiers = 0;

    while (currentTokenLeftIndex < ducky.length()) {
      int nextSpace = ducky.indexOf(' ', currentTokenLeftIndex);
      if (nextSpace==-1){
        currentTokenRightIndex = ducky.length();
      } else {
        currentTokenRightIndex = nextSpace;
      }
      currentToken = ducky.substring(currentTokenLeftIndex, currentTokenRightIndex);
      if (currentToken == "CTRL" || currentToken == "CONTROL"){
        modifiers += KEY_CTRL;
      } else if (currentToken == "SHIFT"){
        modifiers += KEY_SHIFT;
      } else if (currentToken == "ALT") {
        modifiers += KEY_ALT;
      } else if (currentToken.length() != 1) {
        // Search for named key, e.g. DELETE or TAB
        if (keyKnown(currentToken)){
          pressNamedKey(currentToken, modifiers);
          delay(2);
        } else {
          // unknown named key
          ::display.drawString(3,22,String("ERROR"));
          payloadRun.updateDisplay();
          delay(1000);
        }
      } else {
        // Single letter like the 's' in: CTRL s
        unsigned char keycode = keymap[currentToken[0]].usage;
        keyboard.sendPress(keycode, modifiers);
        delay(2);
      }
      currentTokenLeftIndex = currentTokenRightIndex + 1;
    }
    keyboard.sendRelease();
  }
  else {
    Serial.println("Command not found");
  }

  payloadRun.updateDisplay();
}

void rPayload (String payloadRaw) {
    strip.setPixelColor(0, strip.Color(255,0, 0)); 
    strip.show(); strip.show(); strip.show();
    
    String command;
    
    for (int i=0; i < payloadRaw.length(); i++) {
        if (payloadRaw.charAt(i) == '\n') {
          Serial.println(command);
          processDuckyScript(command);
          command = "";
        }
        command+=payloadRaw[i];
    }
    processDuckyScript(command);
    ::display.clear();

    //manual update display
    payloadSelector.updateDisplay();
    ::display.drawXbm(0, 0, 128, 64, high_signal_bits);
    ::display.display();
    strip.setPixelColor(0, strip.Color(0,0, 0)); 
    strip.show(); strip.show();
}

void rPayload (const char* path, uint8_t from) {
    
    FRESULT fr;            
    FIL file; 
    uint16_t size;
    UINT bytesRead;
    
    fr = f_open(&file, path, FA_READ);
     
    if (fr == FR_OK){
        size = f_size(&file);
        char * data = NULL;

        data = (char*) malloc(size);
        Serial.printf("File size: %d bytes", size);

        fr = f_read(&file, data, (UINT) size, &bytesRead);
        if (fr == FR_OK){
            Serial.println("File successfully read!");
            String command;
            
            for (int i=0; i < bytesRead; i++) {
                if (data[i] == 0x0a) {
                  Serial.println(command);
                  processDuckyScript(command);
                  command = "";
                }
                command+=data[i];
            }
            processDuckyScript(command);
        }
        free(data); // free allocated memory when you don't need it

        f_close(&file);
    }

    // web vs local
    if (from==0) {
      ::display.clear();
      ::display.drawXbm(0, 0, 128, 64, high_signal_bits);
      ::display.drawString(3,9,"Press DOWN");
      ::display.drawString(3,19,"to go back");
      ::display.drawLine(0, 54, 127, 54);
      ::display.drawLine(0, 53, 127, 53);
      
      ::display.drawString(0, 54, "FINISHED PAYLOAD");
      ::display.display();
  
      strip.setPixelColor(0, strip.Color(0,255, 0)); 
      strip.show(); strip.show();
  
      
      while (!(nuggButtons.dnPressed())) {
        nuggButtons.getPress();
        vTaskDelay(2);
      }
    }
    

}

// newFileList returns a paginated list of strings representing the files
// available at the given path. resultsPerPage will be set to number of results
// returned. On error, it returns nullptr and numFiles is set to -1.
FILINFO* newFileList(const char* path, int& numFiles) {
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
  numFiles = filesSoFar;
  return fileList;

onError:
  numFiles = -1;
  delete[] fileList;
  return nullptr;
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
bool displayFiles(FILINFO* files, String* currentPath, int numFiles, int selected, int top) {
  if (!files || selected >= numFiles || (top > 0 && numFiles < MAX_FILE_SELECTIONS)) {
    Serial.printf("[displayFiles] condition error: (files:%p)(numFiles:%d)(selected:%d)(top:%d)\n");
    return false;
  }
  ::display.clear();

  // loop condition: display only MAX_FILE_SELECTIONS at a time, and not more
  // than files we actually have.
  for (int i = top; i < top+MAX_FILE_SELECTIONS && i < numFiles; i++) {
    ::display.drawString(6, 10*(i-top), files[i].fname); // TODO: truncate long names
  }
  // Cursor
  ::display.drawRect(2,10*(selected-top)+6,2,2);

  // Footer
  ::display.drawLine(0, 54, 127, 54);
  ::display.drawLine(0, 53, 127, 53);
  ::display.drawString(0,54,"dir:");
  if(currentPath->length() > 17) {
    ::display.drawString(25, 54, currentPath->substring(0,14)+"...");
  } else {
    ::display.drawString(25, 54, *currentPath);
  }
  // Cat image
  ::display.drawXbm(0, 0, 128, 64, RubberNugget_bits);

  // Write to screen
  ::display.display();
  return true;
}

// continue looping until user selects payloads
void RubberNugget::selectPayload() {
  String currentPath("/");
  int numFiles = 0;
  int fileListSelected = 0;
  int fileListTop = 0;

  FILINFO* files = newFileList(currentPath.c_str(), numFiles);
  if (!files) {
    //TODO:display error
    return;
  }
  bool ok = displayFiles(files, &currentPath, numFiles, fileListSelected, fileListTop);
  if (!ok) {
    Serial.println("[selectPayload][1] display files error");
  }
  
  while (true) {
    int press = nuggButtons.getPress();
    if (press==1) { // up
      if (fileListSelected != 0) {
        fileListSelected--;
      }
      if (fileListSelected < fileListTop) {
        fileListTop--;
      }
    } else if (press==2) { // down
      if (fileListSelected < numFiles-1) {
        fileListSelected++;
      }
      if (fileListSelected-fileListTop > MAX_FILE_SELECTIONS-1){
        fileListTop++;
      }
    } else if (press==3) { // left
        int lastSlash = currentPath.lastIndexOf("/");
        if (lastSlash == 0) {
          currentPath = "/";
        } else {
          currentPath = currentPath.substring(0, lastSlash);
        }
        delete []files;
        files = newFileList(currentPath.c_str(), numFiles);
        fileListSelected = 0;
        fileListTop = 0;
    } else if (press==4) { // right
      if(files[fileListSelected].fattrib & AM_DIR){ // directory; enter it
        if (currentPath.length()!=1){
          currentPath += "/";
        }
        currentPath += files[fileListSelected].fname;
        delete[] files;
        files = newFileList(currentPath.c_str(), numFiles);
        fileListSelected = 0;
        fileListTop = 0;
      } else { // file; assume it's a payload and run it
        String payload = currentPath;
        if (payload.length()!=1){
          payload += "/";
        }
        payload += files[fileListSelected].fname;
        runPayload(payload.c_str(), 0);
      }
    } else { // none / unknown btn
      continue;
    }

    ok = displayFiles(files, &currentPath, numFiles, fileListSelected, fileListTop);
    if (!ok) {
      Serial.println("[selectPayload][2] display files error");
    }

  }
  // unreachable, but just in case something changes
  delete[] files;
}

void RubberNugget::runPayload(const char* path, uint8_t from) {  
  // i have no clue why this works
  for (int i=253; i<255; i++) {
    strip.setPixelColor(0, strip.Color(i,0, 0)); 
    strip.show();
  }
  
  ::rPayload(path, from);  
  
  for (int i=253; i<255; i++) {
    strip.setPixelColor(0, strip.Color(0,0, 0)); 
    strip.show();
  }
}

void RubberNugget::runLivePayload(String payloadRaw) {
//  strip.setPixelColor(0, strip.Color(0,0, 255)); strip.show();
  delay(100);
  ::rPayload(payloadRaw); 
}

// allPayloadPaths returns a comma seperated list of paths to all payloads.
String* RubberNugget::allPayloadPaths(const char* path) {
  int numFiles = 0;
  FILINFO* files = newFileList(path, numFiles);

  // DFS on fs
  // TODO: measure max stack usage; if cutting it close reimplement
  // non-recursively.
  if (numFiles < 1) {
    return nullptr;
  }
  String* ret = new String;
  for(int i = 0; i < numFiles; i++) {
    if (files[i].fattrib & AM_DIR) { // Directory; recurse
      String recursivePath(path);
      if (recursivePath.length()>1){ // non-root
        recursivePath += "/";
      }
      recursivePath += files[i].fname;
      String* subDirFiles = allPayloadPaths(recursivePath.c_str());
      if (subDirFiles) {
        (*ret) += (*subDirFiles);
        delete subDirFiles;
      }
    } else { // file, append to list
      // TODO: escape commas in file names 
      String pathString(path);
      if (pathString.length() > 1) { // non-root
        pathString += "/";
      }
      (*ret) += pathString;
      (*ret) += files[i].fname;
      (*ret) += ",";
    }
  }
  delete[] files;
  return ret;
}
