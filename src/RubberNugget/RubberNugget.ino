#include <Adafruit_NeoPixel.h>
#include "RubberNugget.h"
#include "Arduino.h"
#include <base64.h>
#include "base64.hpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include "Webserver.h"
#include "SH1106Wire.h"
#include "Nugget_Interface.h"

// for file streaming
#include "cdcusb.h"
#include "mscusb.h"
#include "flashdisk.h"

extern SH1106Wire display;
extern String rubberCss;
extern String rubberHtml;
extern String rubberJs;
extern String createHtml;

extern String payloadPath;
bool webstuffhappening = false;

extern Nugget_Interface payloadSelector;

Adafruit_NeoPixel strip {1, 12, NEO_GRB + NEO_KHZ400 };

const char *ssid = "RubberNugget";
const char *password = "nugget123";
WebServer server(80);

TaskHandle_t webapp;
TaskHandle_t nuggweb;

void getPayloads() {
  Serial.println(RubberNugget::getPayloads());
  server.send(200, "application/json", RubberNugget::getPayloads());
}

void handleRoot() {
//  Serial.println(ESP.getFreeHeap());
  Serial.println("handling root!");
  server.send(200, "text/html", runHtml);
}

void stylesheet() {
  server.send(200, "text/css", runCss);
}

void create() {
  server.send(200, "text/html", createHtml);
}

void javascript() {
  server.send(200, "text/javascript", runJs);
}

void delpayload() {
  String path = (server.arg("path"));
  char tab2[100];
  strcpy(tab2, path.c_str());
  f_unlink(tab2);
  server.send(200);
}

void websave() {
  String path = (server.arg("path"));
  String content = (server.arg("content"));
  content.replace(" ", "/");

  FRESULT fr;
  FIL file;
  uint16_t size;
  UINT bytesRead;


  FILINFO fno;
  char tab1[100];
  strcpy(tab1, path.substring(0,path.lastIndexOf("/")).c_str());

  fr = f_stat(tab1, &fno);

  if (fr!=FR_OK) {
    Serial.println((char*) tab1);
    f_mkdir(tab1);
  }

  char tab2[100];
  strcpy(tab2, path.c_str());

  char tab3[content.length() + 1];
  strcpy(tab3, content.c_str());

  uint8_t raw[BASE64::decodeLength(tab3)];
  BASE64::decode(tab3, raw);



  fr = f_open(&file, tab2, FA_WRITE | FA_CREATE_ALWAYS);
  if (fr == FR_OK) {
    Serial.println("opened: " + path);
    UINT written = 0;
    fr = f_write(&file, (char*) raw, BASE64::decodeLength(tab3), &written);
    Serial.println(fr);
    f_close(&file);
  }

}

// decode base 64 and run

void webrunlive() {
  if (server.hasArg("plain")) {
    Serial.print("Decoding: ");
    String decoded = (server.arg("plain"));
    char tab2[decoded.length() + 1];
    strcpy(tab2, decoded.c_str());

    for (int i = 0; i < decoded.length(); i++) {
      Serial.print(tab2[i]);
    }

    Serial.println();
    Serial.println(decoded.length());
    Serial.println("-------");

    uint8_t raw[BASE64::decodeLength(tab2)];
    Serial.println(BASE64::decodeLength(tab2));
    BASE64::decode(tab2, raw);

    String meow = (char*) raw;
    meow = meow.substring(0, (int) BASE64::decodeLength(tab2));
    RubberNugget::runLivePayload(meow);
    Serial.println();
    Serial.println("-------");


  }

}

void webget() {
  FRESULT fr;
  FIL file;
  uint16_t size;
  UINT bytesRead;

  String path = server.arg("path");
  char tab2[100];
  strcpy(tab2, path.c_str());

  fr = f_open(&file, tab2, FA_READ);

  if (fr == FR_OK) {
    size = f_size(&file);
    char * data = NULL;

    data = (char*) malloc(size);

    fr = f_read(&file, data, (UINT) size, &bytesRead);
    if (fr == FR_OK) {

      String test = (String) data;
      test = test.substring(0, bytesRead);
      Serial.println(test);

      String encoded = base64::encode(test);
      server.send(200, "plain/text", encoded);
      Serial.println(encoded);
    }
    f_close(&file);
  }

}

// run payload with get request path
void webrun() {
  server.send(200, "text/hhtml", "");
  String path = server.arg("path");
  char tab2[100];
  strcpy(tab2, path.c_str());

  RubberNugget::runPayload(tab2, 1); // provide parameter triggered from webpage
}

void setup() {
  pinMode(12, OUTPUT); delay(500);


  Serial.println(115200);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/data.json", getPayloads);

  server.on("/run.html", handleRoot);
  server.on("/run", handleRoot);
  server.on("/run.js", javascript);
  server.on("/style.css", stylesheet);

  server.on("/savepayload.php", HTTP_POST, websave);
  server.on("/deletepayload.php", HTTP_POST, delpayload);
  server.on("/runlive.php", HTTP_POST, webrunlive);
  server.on("/getpayload.php", HTTP_GET, webget);
  server.on("/runpayload.php", HTTP_GET, webrun);
  server.on("/create.html", create);

  server.begin();

  strip.begin(); strip.clear(); strip.show();
  strip.setPixelColor(0, strip.Color(0, 0, 0)); strip.show();

  strip.setBrightness(100);
  payloadSelector.addNav(RubberNugget::selectPayload);

  // initialize & launch payload selector
  RubberNugget::init();
  xTaskCreate(webserverInit, "webapptask", 12 * 1024, NULL, 5, &webapp); // create task priority 1
  RubberNugget::selectPayload("/");

  // udpates nav map & path infinitely
  payloadSelector.autoUpdateDisplay();

}

void loop() {
  return;
}

void webserverInit(void *p) {
  while (1) {
    server.handleClient();
    vTaskDelay(2);
  }
}
