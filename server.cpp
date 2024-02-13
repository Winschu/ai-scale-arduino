#include "server.h"

#include "paper.h"

#define SECRET_SSID "MARCEL-OMEN-LAP-2970"
#define SECRET_PASS "y28D66:2"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

IPAddress serverIP;

char serverAddress[] = "hs.bandowski.dev";  // server address

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress);

int status = WL_IDLE_STATUS;

int HTTP_PORT = 443;
String HTTP_METHOD = "POST";
char HOST_NAME[] = "hs.bandowski.dev";
String PATH_NAME = "/upload_image";

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
  uint32_t postDataLength = strlen(base64) + strlen(weight) + 1 + 10;

  Serial.print(F("Try to connect to Server: "));
  Serial.println(serverAddress);

  Serial.print(F("Post Data Length: "));
  Serial.println(postDataLength);

  long pingTime = pingServer(HOST_NAME, HTTP_PORT);
  if (pingTime >= 0) {
    Serial.print(F("Ping successful! Response time: "));
    Serial.print(pingTime);
    Serial.println(" ms");
  } else {
    Serial.println("Ping failed!");
  }

  int serverConnection = client.connect(HOST_NAME, HTTP_PORT);

  Serial.print("Server Connection: ");
  Serial.println(serverConnection);

  if (serverConnection) {
    Serial.print(F("Connected to server: "));
    Serial.println(client.connected());

    Serial.println(F("Sending HTTP header..."));
    Serial.println(HTTP_METHOD);

    client.print(HTTP_METHOD);
    client.print(" ");
    client.print(PATH_NAME);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(HOST_NAME);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println();
    client.print("Content-Length: ");
    client.println(postDataLength);
    client.println("Connection: close");

    // Überprüfen, ob die Header erfolgreich gesendet wurden
    if (!client.connected()) {
      Serial.println(F("Fehler beim Senden der Header"));
      return;
    }

    delay(1000);

    Serial.print(F("Free Memory after HTTP Header Data: "));
    Serial.println(freeMemory());

    Serial.println(F("Sending Body data..."));

    client.print("image_data=");
    client.print(base64);
    client.print("&weight=");
    client.println(weight);

    Serial.print(F("Free Memory after HTTP Body Data: "));
    Serial.println(freeMemory());

    // Eine kurze Verzögerung, um sicherzustellen, dass der Body vollständig gesendet wurde
    delay(1000);

    // Body senden

    // Warten, bis Daten vom Server empfangen wurden
    unsigned long timeout = millis() + 5000; // Timeout nach 5 Sekunden
    while (millis() < timeout) {
      if (client.available()) {
        // Daten vom Server empfangen
        break;
      }
    }

    // Überprüfen, ob Daten empfangen wurden oder das Timeout erreicht wurde
    if (millis() >= timeout) {
      Serial.println(F("Timeout beim Warten auf Daten vom Server"));
    } else {
      // Daten erfolgreich empfangen, Verbindung schließen
      client.stop();
      Serial.println(F("Daten erfolgreich empfangen, Verbindung geschlossen"));
    }


    const size_t maxDataLength = 4096;  // Maximal zulässige Länge der empfangenen Daten
    char receivedData[maxDataLength];   // Puffer zum Speichern der empfangenen Daten
    size_t dataIndex = 0;               // Index für das Hinzufügen von Zeichen in den Puffer

    while ((client.connected() || client.available()) && dataIndex < maxDataLength - 1) {
      if (client.available()) {
        char c = client.read();
        receivedData[dataIndex] = c;
        dataIndex++;
      } else {
        Serial.println(F("Client not available anymore"));
      }
    }

    // Nullterminator hinzufügen, um den Puffer als eine gültige Zeichenkette zu beenden
    receivedData[dataIndex] = '\0';

    // Überprüfe, ob Daten empfangen wurden, bevor sie gedruckt werden
    if (dataIndex > 0) {
      Serial.println(receivedData);
    } else {
      Serial.println("No data received!");
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println(F("connection failed on "));
    Serial.print(HOST_NAME);
    Serial.print(" @ ");
    Serial.println(HTTP_PORT);
  }

  if (WiFi.hostByName(serverAddress, serverIP)) {
    Serial.print("Server IP Address: ");
    Serial.println(serverIP.toString());
  } else {
    Serial.println("Failed to resolve server address");
  }
}
