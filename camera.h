#ifndef CAMERA_H
#define CAMERA_H

#include "ArducamLink.h"
#include "Arducam_Mega.h"
#include <SPI.h>
#include "Base64.h"
#include <MemoryFree.h>

void initCamera();
void takePicture();
void readCameraBuffer();
void encodeWithProgress(char* output, char* input, size_t length);
bool isValidBase64(char* base64String);
char* setupCamera();

#endif