#include <Wire.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("BMP280 introuvable : verifie le cablage");
    while (1) delay(10);
  }
}
void loop() {
  Serial.printf("T=%.1f C  P=%.1f hPa\n", bmp.readTemperature(), bmp.readPressure() / 100.0);
  delay(2000);
}