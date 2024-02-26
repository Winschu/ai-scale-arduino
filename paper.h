#ifndef PAPER_H
#define PAPER_H

#include <SPI.h>
#include <stdio.h>
#include <stdint.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <qrcode.h>
#include "Base64.h"
#include <MemoryFree.h>

#define COLORED 0
#define UNCOLORED 1

void setupPaper();
void drawQRCode(char* content);
void displayFrame();

#endif