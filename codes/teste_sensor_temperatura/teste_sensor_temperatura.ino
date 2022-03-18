#include <DHT.h>

#define pin_dht D4
#define tipo_sensor_dht DHT11

int temperatura;

DHT dht(pin_dht, tipo_sensor_dht);

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  temperatura = dht.readTemperature();

  Serial.println(temperatura);

  delay(5000);
}
