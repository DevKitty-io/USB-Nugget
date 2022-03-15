#define up 9 // left button
#define dn 18 // right button
#define lt 11 // left button
#define rt 7 // right button
    
#include "RubberNugget.h"
#include "Arduino.h"

#include "Nugget_Interface.h" // construct Nugget Interface
#include "Nugget_Buttons.h"

#include "SH1106Wire.h"
#include "graphics.h"
#include "keyboardlayout.h"
#include <Adafruit_NeoPixel.h>


extern Adafruit_NeoPixel pixels;

SH1106Wire display(0x3C, 33, 35);

Nugget_Buttons nuggButtons(up,dn,lt,rt);

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

// set up defaults
void RubberNugget::init() {
    pixels.begin(); delay(200);
    pixels.clear();     
    
    pixels.setPixelColor(0, pixels.Color(0,0, 0)); pixels.show();
    pixels.setBrightness(50);
    pixels.show();
    
    
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
    
    const char* osList[] = {"Linux", "Mac", "Windows", "Starred"};
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

void processDuckyScript(String ducky) {
  String tCommand = ducky.substring(0, ducky.indexOf(' ')); // get command
  tCommand.toUpperCase(); tCommand.trim();

  ::display.clear();
  payloadRun.addFooter("RUNNING payload");
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

void rPayload (char* path) {
    FRESULT fr;            
    FIL file; 
    uint16_t size;
    UINT bytesRead;

      
      payloadRun.addFooter(path);
//      payloadRun.addDashboard();
      payloadRun.updateDisplay();
      
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
            Serial.println(command);
        }
        free(data); // free allocated memory when you don't need it

        f_close(&file);
    }
    ::display.clear();
    ::display.drawXbm(0, 0, 128, 64, high_signal_bits);
    ::display.drawString(3,12,"Press any key");
    ::display.drawString(3,22,"to go back");
    ::display.drawLine(0, 54, 127, 54);
    ::display.drawLine(0, 53, 127, 53);
    
    ::display.drawString(0, 54, "FINISHED payload");
    ::display.display();
//    ::pixels.setPixelColor(0, ::pixels.Color(0,150, 0)); ::pixels.show();
//    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(255,0, 0)); pixels.show();
    
    while (!(nuggButtons.dnPressed())) {
      nuggButtons.getPress();
    }
    ::display.clear();
    pixels.setPixelColor(0, pixels.Color(0,0, 0)); pixels.show();
}

String* listDirs(char* path) {

  uint8_t count = 0;
  String* contents = new String[4];

  res = f_opendir(&dir, path);
  if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0 || count>3) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {
                if (res != FR_OK) break;
                contents[count] = (String) fno.fname;
                count++;
                Serial.println(fno.fname);
                Serial.println(count);
            } 
            else {                                       /* It is a file. */
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

      if (payloadPath.indexOf(".txt") !=-1 ) {
        
        char char_array[payloadPath.length()+1];
        payloadPath.toCharArray(char_array, payloadPath.length()+1);

        runPayload(char_array); // calls runPayload with name of path
        payloadPath="";
//        return;
      }
      
      if (payloadPath!="/") {
        payloadPath+= "/";
      }
    }

    // backwards navigation
    else if (strcmp(cpath,"BACK") == 0) {
      payloadPath = payloadPath.substring(0,payloadPath.length()-1); // drop last character
      payloadPath = payloadPath.substring(0,payloadPath.lastIndexOf("/")+1); // drop last path
    }
   
    char char_array[payloadPath.length()];
    payloadPath.toCharArray(char_array, payloadPath.length());
    
    String* dirlisting = listDirs(char_array);
    payloadSelector.addKeyMap(dirlisting);
    payloadSelector.addNav(selectPayload); // pass path 
    payloadSelector.addFooter(payloadPath );
    payloadSelector.autoUpdateDisplay();
    
    
      while (CDCUSBSerial.available()) {
        echo_all(CDCUSBSerial.read());
      }
      
      while (Serial.available()) {
        echo_all(Serial.read());
      }

      delay(0);
}

void RubberNugget::runPayload(char* path) {  
  pixels.setPixelColor(0, pixels.Color(0,0, 255)); pixels.show();
  delay(100);
  ::rPayload(path);
  
}
