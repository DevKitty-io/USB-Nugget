#define up_btn 9 // left button
#define dn_btn 18 // right button
#define lt_btn 11 // left button
#define rt_btn 7 // right button
    
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

// FatFS variables
FRESULT res;
FF_DIR dir;
UINT i;
static FILINFO fno;

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
const char* osList[] = {"Linux", "Mac", "Windows", "Starred"};

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

    // create default folders if they don't exist
    for (int i=0; i<4; i++) {
      res = f_stat(osList[i], &fno);
      if (res!= FR_OK) {
        f_mkdir(osList[i]);
      }
    }    
}

bool keyKnown (String keyPress) {
  Serial.print("looking for: ");
  Serial.println(keyPress);
  for (int i=0; i< (sizeof(keyMapRN)/sizeof(keyMapRN[0])); i++) {
    if (keyPress.equals(keyMapRN[i].title)) {
      Serial.println(keyMapRN[i].title);
      Serial.println(" found!");
      return true;
    }
  }
  return false;
}

void pressKey(String keyPress) {
  delay(2);
  for (int i=0; i< (sizeof(keyMapRN)/sizeof(keyMapRN[0])); i++) {
    if (keyPress.equals(keyMapRN[i].title)) {
      keyboard.sendPress(keyMapRN[i].key);
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
    ::display.drawString(3,12,"KEY PRESS:");
    
    if (String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())).length() > 11) {
      ::display.drawString(3,22,String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())).substring(0,8)+"...");
    }
    else {
      ::display.drawString(3,22,String(ducky.substring(ducky.indexOf(' ')+1, ducky.length())));
    }
    display.drawXbm(0, 0, 128, 64, high_signal_bits);
    payloadRun.updateDisplay();
    delay(defaultDelay*10);
    
    pressKey(tCommand); // press first
    
    String duckyCurrent;
    duckyCurrent = ducky;
    duckyCurrent.toUpperCase();

    Serial.println(duckyCurrent.indexOf(" "));
    
    while (duckyCurrent.indexOf(" ")>-1) {
      
      duckyCurrent = duckyCurrent.substring(duckyCurrent.indexOf(' ')+1, duckyCurrent.length()); // trim off next part of string
      String dCommand = (String) duckyCurrent.substring(0,(duckyCurrent.indexOf(' ')));
      
      if (dCommand.length() == 1) {
        const char* meow = dCommand.c_str();
        uint8_t keycode = (uint8_t) meow[0];

        Serial.print(meow); Serial.print("  ");
        Serial.println(keycode);
        Serial.println(keymap[keycode].usage);

        delay(2);
        keyboard.sendPress(keymap[keycode].usage, 0);
      }
      else {
        pressKey(dCommand);
      }
      
    }
    delay(2);
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

void rPayload (char* path, uint8_t from) {
    
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
// 0=files 1=folders
String* contents2 = new String[30];

String* getFileList(char* path, uint8_t filetype) {

    for (int i=0; i<30; i++) {
      contents2[i] = "";
    }
    uint8_t filecount = 0;
     // up to 30 files / folders i guess

    res = f_opendir(&dir, path);                       /* Open the directory */
    
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            
            if (fno.fattrib & AM_DIR and filetype==0) {                    /* It is a directory */
                if (res != FR_OK) break;
                contents2[filecount] = fno.fname;
                filecount++;
            } else if (!(fno.fattrib & AM_DIR) and filetype==1) {
                contents2[filecount] = fno.fname;
                filecount++;
            }
        }
        f_closedir(&dir);
    }
    return contents2;
}



String* contents = new String[4];
uint8_t depth = 0;

// returns list of directories or files
String* listDirs(char* path) {
  contents[0] = "";
  contents[1] = "";
  contents[2] = "";
  contents[3] = "";
  
  uint8_t count = 0;

  res = f_opendir(&dir, path);
  if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0 || count>3) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR and depth<=2) {
                if (res != FR_OK) break;
                contents[count] = (String) fno.fname;
                count++;
                Serial.println(fno.fname);
                Serial.println(count);
            } 
            else if (!(fno.fattrib & AM_DIR) and depth==3) {                                       /* It is a file. */
                contents[count] = (String) fno.fname;
                count++;
            }
        }
        f_closedir(&dir);
    }

    // fill with blank
    if (count<3) {
      for (int i=count; i<(4-count); i++) {
        contents[i] = "";
      }
    }

    // fix to use internal path
    if (!payloadPath.equals("/")) {
      contents[3] = "BACK";
    }
    
  return contents;
}

// continue looping until user selects payloads
void RubberNugget::selectPayload(char* cpath) {
    // forwards navigation   
    if (strcmp(cpath,"BACK") != 0) {
      
      payloadPath+= cpath;
      
      depth++;
      if (payloadPath.indexOf(".txt") !=-1 ) {
        
        char char_array[payloadPath.length()+1];
        payloadPath.toCharArray(char_array, payloadPath.length()+1);

        runPayload(char_array,0); // calls runPayload with name of path
        payloadPath="";
        ::display.clear();
        Serial.println("finished running payload");
        depth=1;
      }
      
      if (payloadPath!="/") {
        payloadPath+= "/";
      }
      
    }

    // backwards navigation
    else if (strcmp(cpath,"BACK") == 0) {
      depth--;
      payloadPath = payloadPath.substring(0,payloadPath.length()-1); // drop last character
      payloadPath = payloadPath.substring(0,payloadPath.lastIndexOf("/")+1); // drop last path  
    }


    char char_array[100];
    payloadPath.toCharArray(char_array, payloadPath.length());    

    // pass key map to library 

    payloadSelector.addKeyMap(listDirs(char_array));
    
    if (payloadPath.length() > 17) {
      payloadSelector.addFooter(payloadPath.substring(0,14)+"...");
    }
    else {
      payloadSelector.addFooter(payloadPath);
    }    
    ::display.drawXbm(0, 0, 128, 64, RubberNugget_bits);
    ::display.display();
  
  
//    payloadSelector.autoUpdateDisplay();
//    while (CDCUSBSerial.available()) {
//      echo_all(CDCUSBSerial.read());
//    }
//    
//    while (Serial.available()) {
//      echo_all(Serial.read());
//    }
    Serial.println("reached end of selector");
    delay(0);
    Serial.println(depth);
}

void RubberNugget::runPayload(char* path, uint8_t from) {  
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

String RubberNugget::getPayloads() {
  
  Serial.println(ESP.getFreeHeap());
  char * payloadOsTypes[] = {"/Linux","/Mac","/Windows","/Starred"};
  String pathJson = "[";
  bool firstPass;
  char tab2[100];
  String meow;
  
  Serial.println(ESP.getFreeHeap());

  for (int i =0; i<4; i++) {
     for (int j=0; j<30; j++){

        // list subdirs in OS
        if (getFileList(payloadOsTypes[i], 0)[j] == "") break;
//        char tab2[100];
       meow = (String) payloadOsTypes[i] + "/" + getFileList(payloadOsTypes[i], 0)[j]; // path
        strcpy(tab2, meow.c_str());
         
        for (int k=0; k<30; k++) {          
          if (getFileList(tab2, 1)[k] == "") {break;}
                   
          pathJson+="{\"pName\":\""+getFileList(tab2, 1)[k]+"\",\"pCategory\":\""+(String) getFileList(payloadOsTypes[i], 0)[j]+"\",\"pOS\":\""+(String) osList[i]+"\"},";
        }
    }
  }
  
  Serial.println(ESP.getFreeHeap());
  pathJson = pathJson.substring(0,pathJson.length()-1);
  pathJson+="]";  
  Serial.println(ESP.getFreeHeap());
  return pathJson; 
}
