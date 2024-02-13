#include "camera.h"
#include "server.h"

const int MEGA_CS = 6;
Arducam_Mega myCAM(MEGA_CS);
ArducamLink myUart;
uint32_t totalLength;
uint8_t* imageBuffer;

char* finalBase64String;

void initCamera() {
  SPI.begin();
  myUart.arducamUartBegin(115200);
  myCAM.begin();
}

void takePicture() {
  myCAM.setImageQuality(HIGH_QUALITY);
  myCAM.setColorEffect(CAM_COLOR_FX_NONE);
  myCAM.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_MODE_OFFICE);
  myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_DEFAULT);

  // Bildaufnahme
  CamStatus status = myCAM.takePicture(CAM_IMAGE_MODE_128X128, CAM_IMAGE_PIX_FMT_JPG);
  if (status != CAM_ERR_SUCCESS) {
    Serial.println(F("Fehler beim Aufnehmen des Bildes."));
    return;
  }

  delay(1000);
}

void readCameraBuffer() {
  totalLength = myCAM.getTotalLength();

  // Festlegen der Puffergröße auf die minimale erforderliche Größe
  uint32_t bufferSize = min(totalLength, static_cast<uint32_t>(4096));

  imageBuffer = (uint8_t*)malloc(bufferSize);

  if (imageBuffer == NULL) {
    Serial.println(F("Nicht genug Speicher zum Allokieren des Bildpuffers."));
    return;
  }

  uint32_t bytesRead = 0;
  while (bytesRead < totalLength) {
    int read = myCAM.readBuff(imageBuffer + bytesRead, min(static_cast<uint32_t>(200), totalLength - bytesRead));
    if (read < 0) {
      Serial.println(F("Fehler beim Lesen des Bildpuffers."));
      free(imageBuffer);
      return;
    }
    bytesRead += read;

    delay(100);
  }
}

void encodeToBase64(char* output, char* input, size_t length) {
  // Kodieren der gesamten Eingabe auf einmal
  int encodedBytes = Base64.encode(output, input, length);

  // Überprüfen auf Fehler bei der Kodierung
  if (encodedBytes < 0) {
    Serial.println(F("Fehler bei der Base64-Kodierung."));
    return;
  }

  Serial.println(F("Base64-Kodierung abgeschlossen."));
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

char* setupCamera() {
  takePicture();
  readCameraBuffer();

  // Base64-Konvertierung
  size_t imageLength = totalLength;
  int encodedLength = Base64.encodedLength(imageLength);
  char* encodedString = (char*)malloc(encodedLength + 1); // Dynamische Speicherzuweisung

  if (encodedString == nullptr) {
    Serial.println("Nicht genug Speicher für die Base64-Kodierung.");
    return nullptr;
  }

  encodeToBase64(encodedString, reinterpret_cast<char*>(imageBuffer), imageLength);
  free(imageBuffer); // Freigabe des dynamisch allokierten Speichers

  return encodedString;
}





