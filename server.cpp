#include "server.h"
#include "paper.h"

WiFiClient wifi;
HttpClient client = HttpClient(wifi, HOST_NAME);

int wifiStatusLED PROGMEM = WL_IDLE_STATUS;

void setupWifi() {
  // attempt to connect to WiFi network:
  while (wifiStatusLED != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatusLED = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10);
  }

  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());
}

long pingServer(const char* host, int port) {
  WiFiClient wifi;
  unsigned long startTime = millis();
  if (!client.connect(host, port)) {
    return -1;
  }
  unsigned long endTime = millis();

  client.stop();
  return endTime - startTime;
}

char* sendToServer(char* base64, char* weight) {
  long pingTime = pingServer(HOST_NAME, HTTP_PORT);
  if (pingTime >= 0) {
    Serial.print(F("Ping successful! Response time: "));
    Serial.print(pingTime);
    Serial.println(F(" ms"));
  } else {
    Serial.println(F("Ping failed!"));
    return nullptr;
  }

  char base64WithHeader[strlen_P(base64Header) + strlen(base64) + 1];
  strcpy_P(base64WithHeader, base64Header);
  strncat(base64WithHeader, base64, sizeof(base64WithHeader) - strlen(base64Header) - 1);

  // Berechnen Sie die Länge der POST-Daten
  int postDataLength = strlen("image_data=") + strlen(base64WithHeader) + strlen("&weight=") + strlen(weight);

  char postData[postDataLength + 1];  // Array für die POST-Daten
  strcpy(postData, "image_data=");
  strcat(postData, base64WithHeader);
  strcat(postData, "&weight=");
  strcat(postData, weight);

  Serial.print(F("Post Data Length: "));
  Serial.println(strlen(postData));

  client.beginRequest();
  client.post(PATH_NAME);
  client.sendHeader(F("Content-Type"), contentType);
  client.sendHeader(F("Content-Length"), strlen(postData));
  client.beginBody();
  client.print(postData);  // Hier werden die POST-Daten in den Body der Anforderung geschrieben
  client.endRequest();

  Serial.print(F("Free Memory after Client Send: "));
  Serial.println(freeMemory());

  int statusCode = client.responseStatusCode();
  Serial.print(F("Status code: "));
  Serial.println(statusCode);

  char* response = (char*)malloc(client.available() + 1);
  if (response == nullptr) {
    Serial.println("Nicht genug Speicher für die Antwort.");
    return nullptr;
  }

  // Verwenden Sie ein char-Array anstelle eines String-Objekts für die Antwort
  int bytesRead = client.read((uint8_t*)response, client.available());
  response[bytesRead] = '\0';  // Nullterminator hinzufügen

  // Suchen Sie nach dem Beginn des Antwortkörpers und schneiden Sie den HTTP-Overhead ab
  char* bodyStart = strstr(response, "\r\n\r\n");
  if (bodyStart != nullptr) {
    bodyStart += 4;  // Überspringen Sie die Zeilenumbrüche
    memmove(response, bodyStart, strlen(bodyStart) + 1);  // Verschieben Sie den Antwortkörper an den Anfang des Arrays
  }

  Serial.print(F("Response length: "));
  Serial.println(strlen(response));

  return response;
}
