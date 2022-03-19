#include <Scheduler.h>

// GLOBAL : Sensor
#define AVG 10000.0
#define TAXA_GAS_MAX 8.0

// GLOBAL : Sensor
#define PIN_SENSOR_GAS A0

String chave_servico;

String evento_servico;

char *append_str(char *here, String s) {
  int i = 0;
  
  while (*x++ = s[i]) { i++; };
  
  return x-1;
}

char *append_ul(char *x, unsigned long u) {
  char buf[20];
  
  return append_str(x, ultoa(u, buf, 10));
}

char post_rqst[256];
char *p;
char *content_length_here;
char *json_start;
int compi;

class SensorTask : public Task {
public:
  bool smsEnviado;
  float sensorVolt;
  float valorSensor;
  float rsGas;
  float R0;
  float taxa;

protected:
  void enviaSms() {
    if (client.connect("maker.ifttt.com", 80)) {
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

  void alternaAlarme(bool statusAlarme) {
    if (statusAlarme == true) {
      lcd.clear();
      lcd.print("AR CONTAMINADO!");
    } else {
      lcd.clear();
      lcd.print("AR DESCONTAMINADO!");
    }
  }

  void setup() {
    smsEnviado = false;
    
    pinMode(PIN_SENSOR_GAS, INPUT);
  }

  void calculate() {
    valorSensor = analogRead(PIN_SENSOR_GAS);
    
    sensorVolt = (float) valorSensor / 1024 * 5.0;
    
    rsGas = (5.0-sensorVolt) / sensorVolt;
    
    taxa = rsGas / R0;
    
    if (taxa <= TAXA_GAS_MAX) {
      if (smsEnviado == false) {
        enviaSms();
      }
    
      smsEnviado = true;
      
      alternaAlarme(true);
    } else {
      alternaAlarme(false);
    }
  
    delay(2000);
  }

  void loop() {
    calculate();
  }
} sensor_Task;
