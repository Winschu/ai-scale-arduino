#include "server.h"

#define SECRET_SSID "MARCEL-OMEN-LAP-2970"
//#define SECRET_SSID "MARCEL-DESKTOP-0673"
//#define SECRET_PASS "testPasswordXXX"
#define SECRET_PASS "y28D66:2"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

IPAddress serverIP;
IPAddress local_IP(192, 168, 178, 200);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 255);

char serverAddress[] = "hs.bandowski.dev";  // server address

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress);

int status = WL_IDLE_STATUS;

int HTTP_PORT = 443;
String HTTP_METHOD = "POST";
char HOST_NAME[] = "hs.bandowski.dev";
String PATH_NAME = "/upload_image";
String queryString = "?value1=26&value2=70";

void setupWifi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

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

void sendToServer(char* base64, char* weight) {
  Serial.print("Base64 Length: ");
  Serial.println(strlen(base64));

  Serial.print("Weight Length: ");
  Serial.println(strlen(weight));

  Serial.print(F("Free RAM:"));
  Serial.println(freeMemory());

  uint32_t postDataLength = strlen(base64) + strlen(weight) + 1;

  Serial.print(F("Connect to Server: "));
  Serial.println(serverAddress);

  if (client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    client.print(HTTP_METHOD);
    client.print(" ");
    client.print(PATH_NAME);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(HOST_NAME);
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(postDataLength);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();  // end HTTP header

    // send HTTP body
    client.print("image_data=");
    client.print(base64);
    client.print("&weight=");
    client.println(weight);

    String receivedData = ""; // String zum Speichern der empfangenen Daten

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server:
        char c = client.read();
        // add the incoming byte to the receivedData string:
        receivedData += c;
      }
    }

    // Verbindung wurde geschlossen, den gesamten empfangenen Inhalt ausgeben:
    Serial.println("Empfangene Daten:");
    Serial.println(receivedData);

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println("connection failed");
  }

  if (WiFi.hostByName(serverAddress, serverIP)) {
    Serial.print("Server IP Address: ");
    Serial.println(serverIP.toString());
  } else {
    Serial.println("Failed to resolve server address");
  }
}
