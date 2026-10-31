#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "secrets.h"

DHT dht(4, DHT22);
Adafruit_BMP280 bmp;

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

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(21, 22);
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("BMP280 introuvable : verifie le cablage");
    while (1) delay(10);
  }
  connecterWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connecterWiFi();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmp.readPressure() / 100.0;

  if (isnan(t) || isnan(h)) {
    Serial.println("Erreur DHT22");
  } else {
    Serial.printf("T=%.1f C  H=%.1f %%  P=%.1f hPa\n", t, h, p);
  }
  delay(5000);
}