#ifndef SERVER_H
#define SERVER_H

#include "WiFi.h"
#include "WiFiServer.h"
#include "WiFiSSLClient.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <MemoryFree.h>

#define SECRET_SSID "MARCEL-OMEN-LAP-2970"
#define SECRET_PASS "y28D66:2"

const int HTTP_PORT PROGMEM = 443;
const char HTTP_METHOD[] PROGMEM = "POST";

const char base64Header[] PROGMEM = "data:image/jpeg;base64,";
const char PATH_NAME[] PROGMEM = "/upload_image";
const char contentType[] PROGMEM = "application/x-www-form-urlencoded";
const char HOST_NAME[] PROGMEM = "hs.bandowski.dev";

const char ssid[] PROGMEM = SECRET_SSID;  // your network SSID (name)
const char pass[] PROGMEM = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

void setupWifi();
char* sendToServer(char* base64, char* weight);

#endif