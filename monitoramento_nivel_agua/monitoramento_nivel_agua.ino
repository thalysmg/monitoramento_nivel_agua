#include <HardwareSerial.h> 
#define TINY_GSM_MODEM_SIM800

HardwareSerial SerialGSM(1);

TaskHandle_t dobitaobyte;

String carrier = "Sem Rede";

const int RX_PIN_GSM = 4, TX_PIN_GSM = 2;
const int BAUD_RATE = 9600;
const int sensor = 18;
bool enviouSMS = false;
bool buzinaAtiva = false;

//configuracao da buzina
int buzina = 12;
int canal = 0;
int frequencia = 2000;
int resolucao = 10;

void setup() {
  pinMode(sensor, INPUT);
  Serial.begin(9600);
  setupGSM();

  ledcSetup(canal, frequencia, resolucao);
  ledcAttachPin(buzina, canal);
  //timer = millis();
  //xTaskCreatePinnedToCore(sirene, "sirene", 10000, NULL, 1, &dobitaobyte, 0);
}

void loop() {

  int estado = digitalRead(sensor);
  Serial.print("Estado sensor : ");
  Serial.println(estado);
  switch(estado)
  {
  case 0:
    Serial.println("cheio");
    buzinaAtiva = true;
    if (buzinaAtiva) {
      sendSms();
      xTaskCreatePinnedToCore(sirene,"sirene", 10000, NULL, 1, &dobitaobyte,0); 
    }
    delay(600000);
    buzinaAtiva = false;
    break;
  case 1:
    Serial.println("vazio");
    break;
  }
  delay(100);
  
//delay(3600000); "em produção o envio seria feito a cada 1 hr"
  //delay(10000);
  enviouSMS = false;
}

void setupGSM(){
  SerialGSM.begin(BAUD_RATE, SERIAL_8N1, RX_PIN_GSM, TX_PIN_GSM, false);
  delay(1000);

  SerialGSM.println("AT+CMGF=1");
  delay(1000);
  
  SerialGSM.println("AT+CNMI=1,2,0,0,0");
  delay(1000);
}

void sendSms() {
  Serial.println("Enviando SMS...");
  SerialGSM.println("AT+CMGF=1"); //coloca o módulo para o modo SMS
  delay(100);
  SerialGSM.println("AT+CMGS=\"+5583987523433\"\r");
  delay(500);
  SerialGSM.print("SIM800L Funcionando");
   SerialGSM.write(26);
  Serial.println("SMS enviado");
  delay(500);
}

void sirene(void *pvParameters){
  float sinVal;
  int   toneVal;
  for (byte t = 0; t < 10; t++){
    for (byte x = 0; x < 180; x++){
      //converte graus em radianos
      sinVal = (sin(x*(3.1412/180)));
      //agora gera uma frequencia
      toneVal = 2000+(int(sinVal*100));
      //toca o valor no buzzer
      ledcWriteTone(canal, toneVal);
      //Serial.print("*");
      //atraso de 2ms e gera novo tom
      delay(4);
    }
  }
  ledcWriteTone(canal, 0);
  vTaskDelete(NULL);
}
