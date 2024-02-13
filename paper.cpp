#include "paper.h"

#define COLORED 0
#define UNCOLORED 1

unsigned char image[1024];
Epd epd;
Paint paint(image, 0, 0);
QRCode qrcode;

void setupPaper() {
  epd.LDirInit();

  epd.Clear();

  //Überschreibt kompletten Hintergrund der Area
  paint.Clear(UNCOLORED);
}

void filterAlphaNumeric(char* content) {
  char* src = content;
  char* dst = content;

  while (*src) {
    // Behalten Sie nur alphanumerische Zeichen und Leerzeichen bei
    if (isalnum((unsigned char)*src) || *src == ' ') {
      *dst++ = *src;
    }
    src++;
  }
  *dst = '\0';  // Fügen Sie einen Nullterminator hinzu
}

void drawQRCode(char* content) {
  Serial.println(F("Start Drawing"));

  filterAlphaNumeric(content);

  QRCode qrcode;
  uint8_t* qrcodeData = (uint8_t*)malloc(qrcode_getBufferSize(8));
  if (qrcodeData == NULL) {
    Serial.println(F("Failed to allocate memory for qrcodeData"));
    return;
  }

  Serial.println(F("QR Code Init"));

  Serial.print(F("Content Length: "));
  Serial.println(strlen(content));

  Serial.print(F("Free Memory before QR-Code init: "));
  Serial.println(freeMemory());

  const char *data = "Hello, world!";

  qrcode_initText(&qrcode, qrcodeData, 8, ECC_LOW, content);

  Serial.println(F("After Init Text"));

  // Set the total height of the QR code
  const int totalHeight = qrcode.size;  // One row at a time

  // Anzahl der Quadrate in einer Reihe
  const int numSquares = 4;  // Reduced for a smaller size

  // Breite eines Quadrats
  const int squareWidth = 4;  // Reduced for a smaller size

  // Höhe der Quadrate
  const int squareHeight = 4;  // Reduced for a smaller size

  // Adjust the aspect ratio for the QR code modules
  const float aspectRatioX = 1.0;
  const float aspectRatioY = 1.0;

  // Calculate pixel sizes based on the aspect ratio
  const int pixelSizeX = squareWidth * aspectRatioX;
  const int pixelSizeY = squareHeight * aspectRatioY;

  // Loop through each row of the QR code
  for (int y = 0; y < totalHeight; y++) {
    //Serial.print(F("y: "));
    //Serial.println(y);
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

  free(qrcodeData);  // Don't forget to free the memory!

  displayFrame();
}

void displayFrame() {
  // Display the frame
  epd.DisplayFrame();
}