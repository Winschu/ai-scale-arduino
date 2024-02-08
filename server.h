#ifndef SERVER_H
#define SERVER_H

#include "WiFi.h"
#include "WiFiServer.h"
#include "WiFiSSLClient.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <MemoryFree.h>

void setupWifi();
void sendToServer(char* base64, char* weight);

#endif