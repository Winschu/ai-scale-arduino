#include "camera.h"
#include "server.h"

const int MEGA_CS = 6;
Arducam_Mega myCAM(MEGA_CS);
ArducamLink myUart;
uint32_t totalLength;
uint8_t* imageBuffer;

void initCamera() {
  SPI.begin();
  myUart.arducamUartBegin(115200);
  myUart.send_data_pack(7, "Hello Arduino UNO!");
  myCAM.begin();
  myUart.send_data_pack(8, "Mega start!");
}

void takePicture() {
  // Setze Bildqualität
  myCAM.setImageQuality(DEFAULT_QUALITY);

  //myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_4);

  // Bildaufnahme
  CamStatus status = myCAM.takePicture(CAM_IMAGE_MODE_96X96, CAM_IMAGE_PIX_FMT_JPG);
  if (status != CAM_ERR_SUCCESS) {
    Serial.println("Fehler beim Aufnehmen des Bildes.");
    return;
  }

  delay(10000);
}
 
void readCameraBuffer() {
  // Lies den gesamten Bildpuffer
  totalLength = myCAM.getTotalLength();
  imageBuffer = (uint8_t*)malloc(totalLength);  // Verwenden Sie dynamischen Speicher

  if (imageBuffer == NULL) {
    Serial.println("Nicht genug Speicher zum Allokieren des Bildpuffers.");
    return;
  }

  // Funktion zum Bildlesen
  uint32_t bytesRead = 0;
  while (bytesRead < totalLength) {
    int read = myCAM.readBuff(imageBuffer + bytesRead, min(40, totalLength - bytesRead));
    if (read < 0) {
      Serial.println("Fehler beim Lesen des Bildpuffers.");
      free(imageBuffer);
      return;
    }
    bytesRead += read;

    Serial.println("Total Bytes read: ");
    Serial.print(bytesRead);
    Serial.print("\n");

    // Warte auf weitere Daten (optional, je nach Implementierung der Bibliothek)
    delay(100);  // Passe die Verzögerung an, wenn erforderlich
  }

  Serial.print("Image Unread Length after reading: ");
  Serial.println(myCAM.getReceivedLength());
}

void encodeWithProgress(char* output, char* input, size_t length) {
  size_t chunkSize = 8192;  // Größe der Datenblöcke, die auf einmal kodiert werden
  size_t chunks = length / chunkSize;

  for (size_t i = 0; i < chunks; i++) {
    Base64.encode(output + i * chunkSize, input + i * chunkSize, chunkSize);
    Serial.print("Fortschritt: ");
    Serial.print((i + 1) * 100 / chunks);
    Serial.println("%");
  }

  // Verbleibende Daten kodieren
  size_t remaining = length % chunkSize;
  if (remaining > 0) {
    Base64.encode(output + chunks * chunkSize, input + chunks * chunkSize, remaining);
  }

  Serial.println("Base64-Kodierung abgeschlossen.");
}

bool isValidBase64(char* base64String) {
  const char* validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
  for (size_t i = 0; i < strlen(base64String); i++) {
    if (strchr(validChars, base64String[i]) == NULL) {
      return false;
    }
  }
  return true;
}

void setupCamera() {
  takePicture();

  readCameraBuffer();

  // Base64-Konvertierung
  int encodedLength = Base64.encodedLength(totalLength);
  char* encodedString = (char*)malloc(encodedLength + 1);  // Verwenden Sie dynamischen Speicher

  if (encodedString == NULL) {
    Serial.println("Nicht genug Speicher zum Allokieren des kodierten Strings.");
    free(imageBuffer);  // Geben Sie den Bildpuffer frei, bevor Sie die Funktion beenden
    return;
  }

  Serial.println("Getting base64 value...");

  encodeWithProgress(encodedString, reinterpret_cast<char*>(imageBuffer), totalLength);

  // Geben Sie den Bildpuffer frei, da er nicht mehr benötigt wird
  free(imageBuffer);

  Serial.println("Checking base64 for validity...");

  // Überprüfen, ob das base64-Format gültig ist
  if (isValidBase64(encodedString)) {
    Serial.println("Das base64-Format ist gültig.");
  } else {
    Serial.println("Das base64-Format ist ungültig.");
    free(encodedString);  // Geben Sie den kodierten String frei, bevor Sie die Funktion beenden
    return;  // Beenden Sie die Funktion hier, wenn das base64 ungültig ist
  }

  Serial.println("Starting POST request");

  const char base64Header[] PROGMEM = "data:image/jpeg;base64,";

  size_t finalBase64Length = encodedLength + strlen_P(base64Header);
  char finalBase64String[finalBase64Length];

  strcpy_P(finalBase64String, base64Header);
  strcat(finalBase64String, encodedString);  // Use strcat to concatenate

  Serial.print("Base64: ");
  Serial.println(finalBase64String);

  sendToServer(finalBase64String);
}