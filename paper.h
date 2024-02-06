#ifndef PAPER_H
#define PAPER_H

#include <SPI.h>
#include <stdio.h>
#include "epd1in54_V2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include <qrcode.h>
#include "Base64.h"

void setupPaper();
void drawQRCode(char* content);
void displayFrame();

#endif