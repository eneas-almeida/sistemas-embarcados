#include <Scheduler.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#include <SensorTask.h>
#include "CrystalTask.h"
#include "WifiTask.h"

// GLOBAL : PINAGEM
#define PIN_BUZZER D3
#define PIN_LED_LAN D6
#define PIN_LED_ALERTA D7
#define PIN_LED_INT D8
#define PIN_INT D10

// GLOBAL : INTERRUPCAO
void ICACHE_RAM_ATTR calibracao();

// GLOBAL : CLASSES
CrystalTask crystalTask;
WifiTask wifiTask;
SensorTask sensorTask;

WiFiClient client;

void loop() {}

void setup() {
  WiFi.disconnect();
  
  Serial.begin(9600);

  lcd.begin(16, 2);

  // Escalonador de tarefas
  Scheduler.start(&crystalTask);
  Scheduler.start(&wifiTask); 
  Scheduler.start(&sensorTask); 
  
  Scheduler.begin();

  // Interrupcao
  pinMode(PIN_INT, INPUT);

  // Leds
  pinMode(PIN_LED_INT, OUTPUT);
  pinMode(PIN_LED_ALERTA, OUTPUT);
  pinMode(PIN_LED_LAN, OUTPUT);

  // Buzzer
  pinMode(PIN_BUZZER, OUTPUT);

  // Seta nivel logico baixo dos leds
  digitalWrite(PIN_LED_INT, LOW);
  digitalWrite(PIN_LED_ALERTA, LOW);
  digitalWrite(PIN_LED_LAN, LOW);

  // Configura a interrupcao na borda de descida
  attachInterrupt(digitalPinToInterrupt(PIN_INT), calibracao, RISING);

  // Executa a funcao de calibracao
  calibracao();

  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO.");

  for(int i=0; i<=2; i++) {
    delay(1000);
    lcd.print(".");
  }

  delay(1000);

  lcd.clear();
  lcd.print("CONECTANDO");

  WiFi.begin("Jr5G", "penadepato");

  while((!(WiFi.status() == WL_CONNECTED))) {
    delay(2000);
    lcd.print(".");
  }

  lcd.clear();

  digitalWrite(PIN_LED_LAN, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  
  delay(100);
  
  digitalWrite(PIN_BUZZER, LOW);

  lcd.print("CONECTADO!!!");

  delay(3000);

  lcd.clear();
  lcd.print("VERIFICANDO AR");

  for(int i=0; i<=2; i++) {
    delay(2000);
    lcd.print(".");
  }

  lcd.clear();
  lcd.print("AR DESCONTAMINAD");
}

void calibracao() {
  sensorTask.smsEnviado = false;
  
  digitalWrite(PIN_LED_INT, HIGH);

  for(int i=0; i<AVG; i++) {
    sensorTask.valorSensor = sensorTask.valorSensor + analogRead(PIN_SENSOR_GAS);
  }
  
  float rsAir;
  
  sensorTask.valorSensor = sensorTask.valorSensor / AVG;
  sensorTask.sensorVolt = sensorTask.sensorVolt / 1024*5.0; 
  rsAir = (5.0-sensorTask.sensorVolt) / sensorTask.sensorVolt;
  sensorTask.R0 = rsAir / 10.0;
    
  digitalWrite(PIN_LED_INT, LOW);
}
