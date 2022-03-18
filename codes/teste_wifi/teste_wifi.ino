#include <ESP8266WiFi.h>

WiFiClient client;

void setup()
{
  Serial.begin(9600);

  WiFi.disconnect();
  
  WiFi.begin("eneas", "penadepato");
  
  while((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("Conectado!");
}

void loop()
{
  //
}
