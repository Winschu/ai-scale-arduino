#include <SPI.h>
#include <stdio.h>
#include "paper.h"
#include "camera.h"
#include "server.h"
#include "led.h"
#include "matrix.h"


void setupAll() {
  setupPins();
  setupPaper();
  setupWifi();
  setupMatrix();
  initCamera();
}

void setup() {
  Serial.begin(115200);

  Serial.print(F("Free Memory at Start: "));
  Serial.println(freeMemory());

  setupAll();

  Serial.print(F("Free Memory after inital Setup: "));
  Serial.println(freeMemory());

  showDanger();

  char* base64 = nullptr;
  base64 = setupCamera();

  Serial.print(F("Free Memory after Camera: "));
  Serial.println(freeMemory());

  if (base64 == nullptr) {
    Serial.println(F("Base 64 Null-Pointer returnd"));
  }

  char finalWeight[5];

  sendToServer(base64, finalWeight);

  free(base64);
}

void loop() {
  //loopWifi();
}