#include <Adafruit_NeoPixel.h>
#include "RubberNugget.h"
#include "Arduino.h"
#include <base64.h>
#include "base64.hpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include "webUI/index.h"
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
  String* payloadPaths = RubberNugget::allPayloadPaths();
  Serial.printf("[SERVER][getPayloads] %s\n", payloadPaths->c_str());
  server.send(200, "text/plain", *payloadPaths);
}

void handleRoot() {
//  Serial.println(ESP.getFreeHeap());
  Serial.println("handling root!");
  server.send(200, "text/html", String(INDEX));
}

void delpayload() {
  String path(server.arg("path"));
  FRESULT res = f_unlink(path.c_str());
  if (res == FR_OK){
    server.send(200);
  } else {
    server.send(500);
  }
}

void websave() {
  // path should be corrected, i.e. by index.html's pathCorrector function
  // Each path should start with '/' and not end with '/'
  String path = (server.arg("path"));
  const char* constPath = path.c_str();

  // construct directories as needed
  FILINFO filinfo;
  int pathSoFarIdx = 1;
  while(true) {
    int nextDir = path.indexOf("/", pathSoFarIdx);
    if (nextDir == -1){
      break;
    }
    String pathSoFar = path.substring(0, nextDir);
    if (FR_OK != f_stat(pathSoFar.c_str(), &filinfo)){
      if (f_mkdir(pathSoFar.c_str()) != FR_OK) {
        server.send(500, "text/plain", "Could not create directory");
        return;
      }
    }
    pathSoFarIdx = nextDir+1;
  }

  // Create file
  FIL file;
  if (FR_OK != f_open(&file, constPath, FA_WRITE | FA_CREATE_ALWAYS)){
    server.send(500, "text/plain", "Could not open file for writing");
    return;
  }

  // Write to file
  String content = (server.arg("payloadText"));
  content.replace(" ", "/"); // why
  const char* contentBase64 = content.c_str();
  size_t payloadLength = BASE64::decodeLength(contentBase64);
  uint8_t payloadContent[payloadLength];
  BASE64::decode(contentBase64, payloadContent);
  UINT written = 0;
  if (FR_OK != f_write(&file, payloadContent, payloadLength, &written)){
    server.send(500, "text/plain", "Could not write to file");
    f_close(&file);
    return;
  }
  f_close(&file);
  server.send(200, "text/plain", "Payload created successfully");
}

// decode base64 and run
void webrunlive() {
  server.send(200, "text/plain", "Running payload...");
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
  fr = f_open(&file, path.c_str(), FA_READ);

  if (fr != FR_OK) {
    // TODO: most likely file not found, but we need to check why fr != OK.
    // Marking 500 until resolved
    server.send(500, "plain/text", String("Error loading script"));
    return;
  }

  size = f_size(&file);
  char * data = NULL;

  data = (char*) malloc(size);

  fr = f_read(&file, data, (UINT) size, &bytesRead);
  if (fr == FR_OK) {
    String payload = String(data);
    payload = payload.substring(0, bytesRead);
    payload = base64::encode(payload);
    server.send(200, "plain/text", payload);
  } else {
    server.send(500, "plain/text", String("Error reading script"));
  }
  f_close(&file);

}

// run payload with get request path
void webrun() {
  server.send(200, "text/html", "Running payload...");
  String path = server.arg("path");
  RubberNugget::runPayload(path.c_str(), 1); // provide parameter triggered from webpage
}

void webserverInit(void *p) {
  while (1) {
    server.handleClient();
    vTaskDelay(2);
  }
}

void setup() {
  pinMode(12, OUTPUT); delay(500);
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/payloads", getPayloads);
  server.on("/savepayload", HTTP_POST, websave);
  server.on("/deletepayload", HTTP_POST, delpayload);
  server.on("/runlive", HTTP_POST, webrunlive);
  server.on("/getpayload", HTTP_GET, webget);
  server.on("/runpayload", HTTP_GET, webrun);

  server.begin();

  strip.begin(); strip.clear(); strip.show();
  strip.setPixelColor(0, strip.Color(0, 0, 0)); strip.show();
  strip.setBrightness(100);

  // initialize & launch payload selector
  RubberNugget::init();
  xTaskCreate(webserverInit, "webapptask", 12 * 1024, NULL, 5, &webapp); // create task priority 1
  RubberNugget::selectPayload();
}

void loop() {
  return;
}
