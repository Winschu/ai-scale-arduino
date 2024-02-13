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

  char* base64 = setupCamera();

  Serial.print(F("Free Memory after Camera: "));
  Serial.println(freeMemory());

  if (base64 == nullptr) {
    Serial.println(F("Base 64 Null-Pointer returnd"));
  }

  char finalWeight[5];

  char* response = sendToServer(base64, finalWeight);

  free(finalWeight);
  free(base64);

  Serial.print(F("Free Memory after Send To Server: "));
  Serial.println(freeMemory());

  Serial.print(F("Response length before QR-Code: "));
  Serial.println(strlen(response));

  drawQRCode(response);

  free(response);
}

void loop() {
  //loopWifi();
}