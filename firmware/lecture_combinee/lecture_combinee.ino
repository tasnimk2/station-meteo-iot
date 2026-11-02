#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "secrets.h"

DHT dht(4, DHT22);
Adafruit_BMP280 bmp;

const unsigned long INTERVALLE_MS = 60000;  // une mesure par minute
const int MAX_ECHECS = 10;                  // redemarrage apres 10 echecs de suite
int echecs = 0;
bool bmpOK = false;

bool initBMP() {
  delay(100);
  return bmp.begin(0x76) || bmp.begin(0x77);
}

void connecterWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connexion Wi-Fi");
  int essais = 0;
  while (WiFi.status() != WL_CONNECTED && essais < 40) {
    delay(500);
    Serial.print(".");
    essais++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi OK, IP ESP32 : " + WiFi.localIP().toString());
  } else {
    Serial.println("\nEchec Wi-Fi");
  }
}

void envoyerMesures(float t, float h, float p) {
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  String body = "{\"temperature\":" + String(t, 1) +
                ",\"humidity\":" + String(h, 1) +
                ",\"pressure\":" + String(p, 1) + "}";
  int code = http.POST(body);
  Serial.println("Reponse serveur : " + String(code));
  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Cause du dernier redemarrage : %d\n", (int)esp_reset_reason());
  dht.begin();
  Wire.begin(21, 22);
  bmpOK = initBMP();
  if (!bmpOK) Serial.println("BMP280 introuvable : verifie le cablage (nouvel essai a chaque mesure)");
  connecterWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connecterWiFi();
  if (!bmpOK) bmpOK = initBMP();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmpOK ? bmp.readPressure() / 100.0 : NAN;

  bool dhtOK = !isnan(t) && !isnan(h);
  bool pOK = !isnan(p) && p > 950 && p < 1060;

  if (dhtOK && pOK) {
    echecs = 0;
    Serial.printf("T=%.1f C  H=%.1f %%  P=%.1f hPa\n", t, h, p);
    if (WiFi.status() == WL_CONNECTED) envoyerMesures(t, h, p);
    delay(INTERVALLE_MS);
  } else {
    echecs++;
    if (!pOK) bmpOK = false;  // force une nouvelle initialisation du BMP280
    Serial.printf("Mesure invalide : DHT22 %s, BMP280 %s (echec %d/%d)\n",
                  dhtOK ? "ok" : "KO", pOK ? "ok" : "KO", echecs, MAX_ECHECS);
    if (echecs >= MAX_ECHECS) {
      Serial.println("Trop d'echecs : redemarrage");
      delay(1000);
      ESP.restart();
    }
    delay(5000);  // nouvel essai rapide
  }
}