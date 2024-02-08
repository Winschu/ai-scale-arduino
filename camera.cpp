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
  myCAM.begin();
}

void takePicture() {
  myCAM.setImageQuality(HIGH_QUALITY);

  // Bildaufnahme
  CamStatus status = myCAM.takePicture(CAM_IMAGE_MODE_128X128, CAM_IMAGE_PIX_FMT_JPG);
  if (status != CAM_ERR_SUCCESS) {
    Serial.println("Fehler beim Aufnehmen des Bildes.");
    return;
  }

  delay(1000);
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
    int read = myCAM.readBuff(imageBuffer + bytesRead, min(static_cast<uint32_t>(100), totalLength - bytesRead));
    if (read < 0) {
      Serial.println("Fehler beim Lesen des Bildpuffers.");
      free(imageBuffer);
      return;
    }
    bytesRead += read;

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
    Serial.print("Encoded String Length: ");
    Serial.println(strlen(encodedString));
  } else {
    Serial.println("Das base64-Format ist ungültig.");
    free(encodedString);  // Geben Sie den kodierten String frei, bevor Sie die Funktion beenden
    return;  // Beenden Sie die Funktion hier, wenn das base64 ungültig ist
  }

  const char base64Header[] PROGMEM = "data:image/jpeg;base64,";

  size_t finalBase64Length = encodedLength + strlen_P(base64Header);
  char finalBase64String[finalBase64Length];
  char finalWeight[5];

  strcpy_P(finalBase64String, base64Header);
  strcat(finalBase64String, encodedString);  // Use strcat to concatenate

  free(encodedString);

  Serial.println("Starting POST request");

  sendToServer(finalBase64String, finalWeight);
}