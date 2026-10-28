#include <DHT.h>
#define DHTPIN 4
DHT dht(DHTPIN, DHT22);

void setup() {
  Serial.begin(115200);
  dht.begin();
}
void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) Serial.println("Erreur DHT22");
  else Serial.printf("T=%.1f C  H=%.1f %%\n", t, h);
  delay(2000);
}