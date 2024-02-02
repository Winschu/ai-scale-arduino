#ifndef SERVER_H
#define SERVER_H

#include "WiFiS3.h"
#include <ArduinoHttpClient.h>

void setupWifi();
void sendToServer(char* finalBase64String);

#endif