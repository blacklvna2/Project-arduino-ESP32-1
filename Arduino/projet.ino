#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiProv.h>

// Affectation des broches
#define RST_PIN A0
#define SS_PIN D4
MFRC522 mfrc522(SS_PIN, RST_PIN);
#define LedV A1
#define LedR A2
#define btn A3

#define URL "http://badges-api.glitch.me/validate/"
#define URL2 "http://badges-api.glitch.me/add/"

String UID= "";

bool httpRequestSent = false;

const char * pop = "abcd1234";
const char * service_name = "PROV_YOmec";
const char * service_key = NULL;
bool reset_provisioned = true;

void setup() {
 // Initialisation du Module RFID
  Serial.begin(115200);

  Serial.println("start prov");
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name, service_key);
  
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial(); // Affichage des données de la bibliothèque
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
  
  pinMode(LedV, OUTPUT);
  pinMode(LedR, OUTPUT);
  digitalWrite(LedV, LOW);
  digitalWrite(LedR, LOW);
}



void loop() {
  // Récupération UID de carte
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Show UID on serial monitor
    UID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
       UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
       UID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    UID.toUpperCase();
    httpRequestSent = false;
  }

  // add card if btn push
  if (digitalRead(btn)==HIGH){
    HTTPClient http;
    http.begin(URL2 + UID);
    http.GET();
    http.end();
    http.begin(URL + UID);
    http.GET();
    http.end();
  }


  //Utilisation de l'api
  if (UID.length() > 0 && !httpRequestSent) {
    HTTPClient http;
    http.begin(URL + UID);
    int code = http.GET();
    if (code == 200) {
      digitalWrite(LedV, HIGH);
      delay(1000);
      digitalWrite(LedV, LOW);

    }
    else if (code == 403) {
      digitalWrite(LedR, HIGH);
      delay(1000);
      digitalWrite(LedR, LOW);
    }
    else {
      Serial.println(code);
    }
    http.end();

    httpRequestSent = true;
  }

}

