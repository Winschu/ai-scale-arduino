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

  setupAll();

  showDanger();

  setupCamera();

  char value[5] = "test";

  drawQRCode(value);

  showHeart();

  displayFrame();
}

void loop() {
  //loopWifi();
}