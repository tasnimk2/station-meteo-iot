#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

DHT dht(4, DHT22);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(21, 22);
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("BMP280 introuvable : verifie le cablage");
    while (1) delay(10);
  }
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmp.readPressure() / 100.0;

  if (isnan(t) || isnan(h)) {
    Serial.println("Erreur DHT22");
  } else {
    Serial.printf("T=%.1f C  H=%.1f %%  P=%.1f hPa\n", t, h, p);
  }
  delay(2000);
}