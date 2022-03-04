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
SH1106Wire display(0x3C, 33, 35);

Nugget_Buttons nuggButtons(up,dn,lt,rt);

// ESPTinyUSB libraries
#include "cdcusb.h"
#include "mscusb.h"
#include "flashdisk.h"

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

    display.init();
    display.flipScreenVertically();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(DejaVu_Sans_Mono_10);

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


// return 4
String* listDirs(char* path) {

  uint8_t count = 0;
  String* contents = new String[4];

//  contents[0] = "Meow";

  res = f_opendir(&dir, path);
  if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0 || count>3) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {  
//              Serial.println(fno.fname); /* It is a directory */
              
//                i = strlen(path);
//                sprintf(&path[i], "/%s", fno.fname);
//                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                contents[count] = (String) fno.fname;
//                contents[count] = count;
                count++;
                Serial.println(fno.fname);
                Serial.println(count);
//                path[i] = 0;
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
        
        char char_array[payloadPath.length()];
        payloadPath.toCharArray(char_array, payloadPath.length());
    
        runPayload(char_array);
        return;
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
    payloadSelector.updateDisplay();
    
    
      while (CDCUSBSerial.available()) {
        echo_all(CDCUSBSerial.read());
      }
      while (Serial.available()) {
        echo_all(Serial.read());
      }

      delay(0);
//    }
}

void RubberNugget::runPayload(char* path) {
  display.drawString(54,0,"fuck you");
  display.display();
}
