#include "Arduino_LED_Matrix.h"
#include "matrix.h"
#include "matrixFrames.h"

ArduinoLEDMatrix matrix;

void setupMatrix() {
  matrix.begin();
}

void showDanger() {
  matrix.loadFrame(danger);
}

void showHeart() {
  matrix.loadFrame(heart);
}