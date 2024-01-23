#include <qrcode.h>

#include <SPI.h>

#include "epd1in54_V2.h"

#include "imagedata.h"

#include "epdpaint.h"

#include <stdio.h>


Epd epd;

unsigned char image[1024];

Paint paint(image, 0, 0);

unsigned long time_start_ms;

unsigned long time_now_s;

#define COLORED 0

#define UNCOLORED 1

//sensors
int sensorPin = A0;

void setupPaper() {
  Serial.println("e-Paper init and clear");

  epd.LDirInit();

  epd.Clear();

  //Überschreibt kompletten Hintergrund der Area
  paint.Clear(UNCOLORED);
}

void setupWifi() {
}

void setupMatrix() {
}

void setupAll() {
  setupPaper();
  setupWifi();
  setupMatrix();
}

void drawQRCode() {
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];

  qrcode_initText(&qrcode, qrcodeData, 3, 0, "Test");

  // Set the total height of the QR code
  int totalHeight = qrcode.size;  // One row at a time

  // Anzahl der Quadrate in einer Reihe
  int numSquares = 5;  // Reduced for a smaller size

  // Breite eines Quadrats
  int squareWidth = 5;  // Reduced for a smaller size

  // Höhe der Quadrate
  int squareHeight = 5;  // Reduced for a smaller size

  // Adjust the aspect ratio for the QR code modules
  float aspectRatioX = 1.0;
  float aspectRatioY = 1.0;

  // Calculate pixel sizes based on the aspect ratio
  int pixelSizeX = squareWidth * aspectRatioX;
  int pixelSizeY = squareHeight * aspectRatioY;

  // Loop through each row of the QR code
  for (int y = 0; y < totalHeight; y++) {
    // Set the current draw area for one row
    paint.SetWidth(200);
    paint.SetHeight(pixelSizeY);

    // Loop through each module in the row
    for (int x = 0; x < qrcode.size; x++) {
      // Get the module value (black or white)
      bool isColored = qrcode_getModule(&qrcode, x, y);

      // Determine the coordinates on the e-Paper display
      int startX = x * pixelSizeX;  // Adjust pixelSizeX based on your needs
      int endX = startX + pixelSizeX - 1;

      // Schleife zum Zeichnen der Quadrate
      for (int i = 0; i < numSquares; i++) {
        // Wechsel zwischen COLORED und UNCOLORED based on the QR code data
        if (isColored) {
          paint.DrawFilledRectangle(startX, 0, endX, pixelSizeY - 1, COLORED);
        } else {
          paint.DrawFilledRectangle(startX, 0, endX, pixelSizeY - 1, UNCOLORED);
        }
      }
    }

    // Set the position to display the frame
    int yPos = y * pixelSizeY;
    epd.SetFrameMemory(paint.GetImage(), 0, yPos, paint.GetWidth(), paint.GetHeight());
  }
}

void setup() {
  Serial.begin(115200);

  setupAll();

  drawQRCode();

  // Display the frame
  epd.DisplayFrame();
}

void loop() {
}