#include <Adafruit_NeoPixel.h>
#include "src/RubberNugget.h"
#include "Arduino.h"
#include <base64.h>
#include "base64.hpp"

#include <WiFiClient.h>
#include <WebServer.h>

#include "webUI/index.h"

#include "src/utils.h"
#include "src/interface/screens/splash.h"
#include "src/interface/screens/dir.h"
#include "src/interface/screens/runner.h"
#include "src/interface/lib/NuggetInterface.h"

const char *ssid = "Nugget AP";
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
  fileOp decodeOp = base64Decode(server.arg("payloadText"));
  if (!decodeOp.ok){
    server.send(500, "text/plain", decodeOp.result);
    return;
  }
  fileOp saveOp = saveFile(server.arg("path"), decodeOp.result);
  if (!saveOp.ok){
    server.send(500, "text/plain", saveOp.result);
    return;
  }
  server.send(200, "text/plain", "payload saved successfully");
}

void webget() {
  String path = server.arg("path");
  fileOp op = readFile(path);
  if (!op.ok) {
    // TODO: send 500/4XX depending on file existence vs internal error
    server.send(500, "text/plain", String("error getting payload: ") + op.result);
    return;
  }
  String payload = base64::encode(op.result);
  server.send(200, "text/plain", payload);
}

NuggetInterface* nuggetInterface;

// run payload with get request path
void webrun() {
  fileOp op = readFile(server.arg("path"));
  if (op.ok) {
    server.send(200, "text/html", "Running payload...");
    NuggetScreen* runner = new ScriptRunnerScreen(op.result);
    bool ok = nuggetInterface->injectScreen(runner);
    return;
  }
  server.send(500, "text/html", "couldn't run payload: " + op.result);
}

void webrunlive() {
  // TODO: use server.arg "content" or "payload" instead of "plain"
  fileOp op = base64Decode(server.arg("plain"));
  if (op.ok) {
    server.send(200, "text/plain", "running live payload");
    NuggetScreen* runner = new ScriptRunnerScreen(op.result);
    bool ok = nuggetInterface->injectScreen(runner);
    // TODO: send 503 when device is busy
    return;
  }
  server.send(500, "text/html", "Device busy");
}

void webserverInit(void *p) {
  while (1) {
    server.handleClient();
    vTaskDelay(2);
  }
}

void setup() {
  Serial.begin(115200);

  RubberNugget::init();
 
  server.on("/", handleRoot);
  server.on("/payloads", getPayloads);
  server.on("/savepayload", HTTP_POST, websave);
  server.on("/deletepayload", HTTP_POST, delpayload);
  server.on("/runlive", HTTP_POST, webrunlive);
  server.on("/getpayload", HTTP_GET, webget);
  server.on("/runpayload", HTTP_GET, webrun);

  server.begin();

  xTaskCreate(webserverInit, "webapptask", 12 * 1024, NULL, 5, &webapp); // create task priority 1
  nuggetInterface = new NuggetInterface;
  NuggetScreen* dirScreen = new DirScreen("/");
  NuggetScreen* splashScreen = new SplashScreen(1500);
  nuggetInterface->pushScreen(dirScreen);
  nuggetInterface->pushScreen(splashScreen);
  nuggetInterface->start();

}

void loop() { return; }
