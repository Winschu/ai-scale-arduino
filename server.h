#ifndef SERVER_H
#define SERVER_H

#include "WiFi.h"
#include "WiFiServer.h"
#include "WiFiSSLClient.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <MemoryFree.h>

const char base64Header[] PROGMEM = "data:image/jpeg;base64,";

void setupWifi();
void sendToServer(char* base64, char* weight);

#endif