#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

WiFiClient client;

String chave_servico;

String evento_servico;

char *append_str(char *here, String s) {
  int i=0;
  
  while (*here++ = s[i]){i++;};
  
  return here-1;
}

char *append_ul(char *here, unsigned long u) {
  char buf[20];
  return append_str(here, ultoa(u, buf, 10));
}

char post_rqst[256];
char *p;
char *content_length_here;
char *json_start;
int compi;

// PINAGEM
#define PIN_SENSOR_GAS A0
#define PIN_SENSOR_TMP D4
#define PIN_BUZZER D3
#define PIN_DHT D4
#define PIN_LED_LAN D6
#define PIN_LED_ALERTA D7
#define PIN_LED_INT D8
#define PIN_INT D10
#define TIPO_SENSOR DHT11

DHT dht(PIN_DHT, TIPO_SENSOR);

// VARIAVEIS
#define AVG 10000.0
#define TAXA_GAS_MAX 8.0
#define TAXA_TEMP_MAX 35

// GLOBAIS
float sensor_volt;
float valorSensor;
float RS_gas;
float R0;
float taxa;

// GLOBAL
bool sms_enviado = false;
int temperatura;

void ICACHE_RAM_ATTR calibracaoX();

void envia_sms()
{
  if (client.connect("maker.ifttt.com", 80))
  {
    chave_servico = "cTRgedBgpB_uka5vJfFoSZ";
    evento_servico = "sms_micro";
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, evento_servico);
    p = append_str(p, "/with/key/");
    p = append_str(p, chave_servico);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, "ALERTA DE GAS!!!");
    p = append_str(p, "\",\"value2\":\"");
    p = append_str(p, "ALERTA DE GAS!!!");
    p = append_str(p, "\",\"value3\":\"");
    p = append_str(p, "");
    p = append_str(p, "\"}");

    compi= strlen(json_start);
    content_length_here[0] = '0' + (compi/10);
    content_length_here[1] = '0' + (compi%10);
    client.print(post_rqst);
    Serial.println("SMS enviado!!!");
    Serial.println(post_rqst);
  }
}

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();

  WiFi.disconnect();

  sms_enviado = false;

  // Entradas
  pinMode(PIN_SENSOR_GAS, INPUT);
  pinMode(PIN_SENSOR_TMP, INPUT);
  pinMode(PIN_INT, INPUT);

  // Leds
  pinMode(PIN_LED_INT, OUTPUT);
  pinMode(PIN_LED_ALERTA, OUTPUT);
  pinMode(PIN_LED_LAN, OUTPUT);

  // Buzzer
  pinMode(PIN_BUZZER, OUTPUT);

  // Nivel baixo dos leds
  digitalWrite(PIN_LED_INT, LOW);
  digitalWrite(PIN_LED_ALERTA, LOW);
  digitalWrite(PIN_LED_LAN, LOW);

  // Seta a interrupcao
  attachInterrupt(digitalPinToInterrupt(PIN_INT), calibracaoX, RISING);

  // Executa a funcao de calibracao
  calibracaoX();
  
  lcd.setBacklight(HIGH);
  
  lcd.setCursor(0, 0);
  
  lcd.print("INICIANDO.");

  for(int i=0; i<=2; i++)
  {
    delay(1000);
    lcd.print(".");
  }
  
  delay(1000);

  lcd.clear();

  lcd.print("CONECTANDO");
 
  WiFi.begin("eneas", "penadepato");
  
  while((!(WiFi.status() == WL_CONNECTED)))
  {
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

  for(int i=0; i<=2; i++)
  {
    delay(2000);
    lcd.print(".");
  }

  lcd.clear();
  
  lcd.print("AR DESCONTAMINAD");
}

void loop()
{
  valorSensor = analogRead(PIN_SENSOR_GAS);
  temperatura = dht.readTemperature();
  
  sensor_volt = (float) valorSensor / 1024*5.0;
  RS_gas = (5.0-sensor_volt) / sensor_volt;
  taxa = RS_gas / R0;
  
  if (taxa <= TAXA_GAS_MAX)
  {

    if (sms_enviado == false)
    {
      envia_sms();
    }
  
    sms_enviado = true;
    
    alarme("gas", true);
  }
  else
  {
    alarme("gas", false);

    if (isnan(temperatura))
    {
      temperatura = 100;
    }
    else
    { 
      if (temperatura >= TAXA_TEMP_MAX && temperatura < 100)
      {
        alarme("temperatura", true); 
      }
      else
      {
        alarme("temperatura", false); 
      }
    }
  }

  delay(2000);
}

void calibracaoX()
{
  sms_enviado = false;
  
  int i;
  float RS_air;

  digitalWrite(PIN_LED_INT, HIGH);

  for(i = 0; i < AVG; i++)
  {
    valorSensor = valorSensor + analogRead(PIN_SENSOR_GAS);
  }
  
  valorSensor = valorSensor / AVG;
 
  sensor_volt = valorSensor / 1024*5.0;
  
  RS_air = (5.0-sensor_volt) / sensor_volt;
  
  R0 = RS_air / 10.0;
    
  digitalWrite(PIN_LED_INT, LOW);
}

void alarme(String tipo_alarme, bool status_alarme)
{
  if (tipo_alarme == "gas" and status_alarme == true)
  {
    lcd.clear();
    lcd.print("AR CONTAMINADO!");
    saidas(true);
  }
  else if (tipo_alarme == "gas" and status_alarme == false)
  {
    lcd.clear();
    lcd.print("AR DESCONTAMINADO!");
    saidas(false);
  }
  else if (tipo_alarme == "temperatura" and status_alarme == true)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AR DESCONTAMINADO!");
    lcd.setCursor(0,1);
    lcd.print(temperatura);
    lcd.print("C (ALERTA!!!)");    
    saidas(true);
  }
  else if (tipo_alarme == "temperatura" and status_alarme == false)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AR DESCONTAMINADO!");
    lcd.setCursor(0,1);
    lcd.print(temperatura);
    lcd.print("C");
    saidas(false);
  }
}

void saidas(bool status_saida)
{
  if (status_saida == true)
  {
    digitalWrite(PIN_LED_ALERTA, HIGH);
    digitalWrite(PIN_BUZZER, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED_ALERTA, LOW);
    digitalWrite(PIN_BUZZER, LOW);
  }
}
