#include "server.h"
#include "paper.h"

#define SECRET_SSID "MARCEL-OMEN-LAP-2970"
#define SECRET_PASS "y28D66:2"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

char serverAddress[] = "hs.bandowski.dev";  // server address

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress);

int status = WL_IDLE_STATUS;

int HTTP_PORT = 443;
String HTTP_METHOD = "POST";
char HOST_NAME[] = "hs.bandowski.dev";
String PATH_NAME = "/upload_image";

void setupWifi() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

long pingServer(const char* host, int port) {
  WiFiClient client;

  unsigned long startTime = millis();
  if (!client.connect(host, port)) {
    return -1;
  }
  unsigned long endTime = millis();

  client.stop();
  return endTime - startTime;
}

void sendToServer(char* base64, char* weight) {
  long pingTime = pingServer(HOST_NAME, HTTP_PORT);
  if (pingTime >= 0) {
    Serial.print(F("Ping successful! Response time: "));
    Serial.print(pingTime);
    Serial.println(" ms");
  } else {
    Serial.println("Ping failed!");
  }

  Serial.println("making POST request");
  char contentType[] = "application/x-www-form-urlencoded";

  // Ausgabe der Längen von base64 und weight
  Serial.print("Length of base64: ");
  Serial.println(strlen(base64));
  Serial.print("Length of weight: ");
  Serial.println(strlen(weight));

  // Fügen Sie den base64Header aus PROGMEM hinzu
  char base64WithHeader[strlen_P(base64Header) + strlen(base64) + 1];
  strcpy_P(base64WithHeader, base64Header);
  strcat(base64WithHeader, base64);

  // Berechnen Sie die Länge der POST-Daten
  int postDataLength = strlen("image_data=") + strlen(base64WithHeader) + strlen("&weight=") + strlen(weight);

  char postData[postDataLength + 100];  // Array für die POST-Daten

  // Formatieren Sie die POST-Daten mit sprintf
  sprintf(postData, "image_data=%s&weight=%s", base64WithHeader, weight);

  Serial.print(F("Post Data Length: "));
  Serial.println(strlen(postData));

  client.beginRequest();
  client.post(PATH_NAME);
  client.sendHeader("Content-Type", contentType);
  client.sendHeader("Content-Length", strlen(postData));
  client.beginBody();
  client.print(postData);  // Hier werden die POST-Daten in den Body der Anforderung geschrieben
  client.endRequest();

  int statusCode = client.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);

  // Verwenden Sie ein char-Array anstelle eines String-Objekts für die Antwort
  char response[client.available() + 1];
  int bytesRead = client.read((uint8_t*)response, client.available());
  response[bytesRead] = '\0';  // Nullterminator hinzufügen
  Serial.print("Response: ");
  Serial.println(response);

  //delay(10000);
}
