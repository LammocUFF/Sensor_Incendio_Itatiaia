
#include <DS3231.h>
#include <SD.h> 
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 7       // PINO DO DHT22
#define DHTTYPE DHT22  // TIPO DO DHT
#define mq2io 6        // PINO I/O DO MQ2 (TRANSISTOR)
#define mq2a A0        // PINO ANALÓGICO DO MQ2
#define M0 8           // PINO DE MODO DO LORA
#define M1 9           // PINO DE MODO DO LORA
#define SID 4       // NÚMERO DE IDENTIFICAÇÃO DO SENSOR

int aquecimento = 10*1000;           // TEMPO DE AQUECIMENTO DO MQ2
int standby = 10*60*1000;            // TEMPO DE ESPERA ENTRE LEITURAS
int leitura = 10*1000;               // TEMPO DE LEITURA DE DADOS
int samples = 50;                    // QUANTIDADE DE AMOSTRAS COLETADAS
int intervalo = leitura / samples;   // INTERVALO ENTRE AMOSTRAS

File arquivoCSV;

SoftwareSerial lora(2, 3); //TX, RX

// PINAGEM LORA: 
// GND
// GND
// 3
// 2
// ND              ANTENA
// 3.3V            ANTENA
// GND             ANTENA

// PINAGEM RTC
// SDA - A4
// SCL - A5

// PINAGEM MÓDULO SD
// CS - 10
// SCK - 13
// MOSI - 11
// MISO - 12

long id = 1; // contador

DS3231  rtc(SDA, SCL);
DHT dht (DHTPIN, DHTTYPE);

void setup() {
   Serial.begin(9600);
   lora.begin(9600);
   dht.begin();
   rtc.begin();

  // rtc.setDOW(SATURDAY);     // SET DAY OF WEEK
  // rtc.setTime(21, 55, 00);     // SET TIME (FORMATO 12:00:00) (24hr format)
  // rtc.setDate(24, 07, 2021);   // SETAR DATE (DIA_MÊS_ANO)

   pinMode(mq2io, OUTPUT);
   digitalWrite(mq2io, LOW);
   pinMode(M1, OUTPUT);    // M1
   pinMode(M0, OUTPUT);    // M0
   digitalWrite(M1, LOW);  // Esses pinos setam o modo do lora, mais detalhes no datasheet (E32 TTL100)
   digitalWrite(M0, LOW);  // LOW,LOW = normal ; HIGH,LOW = wake-up ; LOW,HIGH = power-save ; HIGH,HIGH = sleep

   SD.begin(10); // the parameter is the pin conneted to the cs
   
   File arquivoCSV = SD.open("dados.csv", FILE_WRITE);
  if (arquivoCSV) {
    arquivoCSV.println("TESTE DE ARQUIVOS TXT EM SD NO ARDUINO.");
    arquivoCSV.println("  "); // blank line in case there was something in the file
    String header = "ID; MQ2; Dia da Semana(); Data(); Hora(); U; T";
    arquivoCSV.println(header);
    Serial.println(F("CARTAO SD OK"));
    arquivoCSV.close();
    //Serial.println("OK.");
  } else {
    Serial.println(F("ERRO AO ABRIR OU CRIAR O ARQUIVO TEXTO.TXT."));
  }
  
}

void loop() {

  // RESET DE VARIÁVEIS
 float mq2 = 0;
 float temp = 0;
 float umid = 0;
  // RESET DE VARIÁVEIS
  
  //----------------------------------------
  
  // LIGA MQ2 E AGUARDA AQUECIMENTO
  Serial.println("MQ2 on");
  digitalWrite(mq2io, HIGH);

  delay(aquecimento);

  
  // LIGA MQ2 E AGUARDA AQUECIMENTO
  
  //----------------------------------------
  
  // VERIFICA SE O DHT22 ESTÁ RESPONDENDO
  if (isnan(umid) || isnan(temp))
  {
    Serial.println("Falha ao ler dados do sensor DHT !!!");
    return;
  }
  // VERIFICA SE O DHT22 ESTÁ RESPONDENDO
  
  //----------------------------------------

  // LEITURA DE DADOS DO MQ2 E DHT22
  for (int i=1; i<=samples; i++){
  mq2 += analogRead(mq2a);
  temp += dht.readTemperature();
  umid += dht.readHumidity();
  delay(intervalo);
  }
  mq2  /= samples;
  temp /= samples;
  umid /= samples;
  mq2  *= 100;
  temp *= 100;
  umid *= 100;
  long imq2 = mq2;
  long itemp = temp ;
  long iumid = umid ;
  Serial.print("SID: ");
  Serial.println(SID);
  Serial.print("MQ2: ");
  Serial.println(imq2);
  Serial.print("Temp: ");
  Serial.println(itemp);
  Serial.print("Umid: ");
  Serial.println(iumid);
  // LEITURA DE DADOS DO MQ2 E DHT22
    
  //----------------------------------------
  
  // DESLIGA MQ2
  Serial.println("MQ2 off");
  digitalWrite(mq2io, LOW);
    
  //----------------------------------------
  
  // ENVIO DE DADOS VIA RÁDIO;
  lora.print("((");
  lora.print(SID);
  lora.print("/");
  lora.print(imq2);
  lora.print("/");
  lora.print(itemp);
  lora.print("/");
  lora.print(iumid);
  lora.print("))");
    
  //----------------------------------------

  // SALVA DADOS NO CARTÃO SD
  
  Serial.println(F("SALVANDO DADOS..."));
  File arquivoCSV = SD.open("dados.csv", FILE_WRITE);
  if (arquivoCSV) {
   //Serial.print("OK...");
   //    String header = "ID; MQ2; Dia da Semana(); Data(); Hora(); U; T";
   arquivoCSV.print(id);
   arquivoCSV.print("; ");
   arquivoCSV.print(mq2);
   arquivoCSV.print("; ");
   arquivoCSV.print(rtc.getDOWStr());
   arquivoCSV.print("; ");
   arquivoCSV.print(rtc.getDateStr());
   arquivoCSV.print("; ");
   arquivoCSV.print(rtc.getTimeStr());
   arquivoCSV.print("; ");
   arquivoCSV.print(umid);
   arquivoCSV.print("; ");
   arquivoCSV.println(temp);
    
   arquivoCSV.close(); // close the file:
   Serial.println(F("ARQUIVO FECHADO."));
   }
   else {
   // if the file didn't open, report an error:
   Serial.println(F("ERRO AO ABRIR O ARQUIVO DE TEXTO!"));
    }
    
  // SALVA DADOS NO CARTÃO SD
    
  //----------------------------------------

  // INCREMENTA CONTADOR E AGUARDA
  id++;

  delay(standby);
}
