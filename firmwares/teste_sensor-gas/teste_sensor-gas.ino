#define pin_sensor_gas A0

int g_max = 25;
int t_max = 35;
int gas, gas_map, temperatura;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  gas = analogRead(pin_sensor_gas);
  gas_map = map(gas, 0, 1023, 0, 100);

  Serial.println(gas_map);

  delay(5000);
}
