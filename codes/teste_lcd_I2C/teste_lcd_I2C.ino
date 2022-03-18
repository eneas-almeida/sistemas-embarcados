#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
  lcd.setBacklight(1);
  lcd.begin (16,2);
  lcd.setCursor(0, 0);
  lcd.print("OK 2.");
}


void loop()
{
  //
}
