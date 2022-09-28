#include <ESP8266WiFi.h> //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
#include <SoftwareSerial.h> 
/* defines - wi-fi */
#define SSID_REDE "ENG_TER_2G" /* coloque aqui o nome da rede que se deseja conectar */
#define SENHA_REDE "engenharia235" /* coloque aqui a senha da rede que se deseja conectar */
#define INTERVALO_ENVIO_THINGSPEAK 10000 /* intervalo entre envios de dados ao ThingSpeak (em ms) */

#define LED_PLACA 16
 
 
/* constantes e variáveis globais */
char endereco_api_thingspeak[] = "api.thingspeak.com";
String chave_escrita_thingspeak = "1KN92BJ3PJIITW0B";  /* Coloque aqui sua chave de escrita do seu canal */
unsigned long last_connection_time;

WiFiClient client;
SoftwareSerial loraSerial(D2, D3); // TX, RX
char fields_a_serem_enviados[100] = {0};
float temperatura_lida = 25.0;
float umidade_lida = 60.0;
float alerta = 1;
float estação = 3;
int ID = 1;
float fumaca1 = 0.0;
float fumaca2 = 0.0;
float fumaca3 = 0.0;
float fumaca4 = 0.0;
float fumaca5 = 0.0;
int temp=0, umidade=0, fumaca = 0;



/* prototypes */
void envia_informacoes_thingspeak(String string_dados);
void init_wifi(void);
void conecta_wifi(void);
void verifica_conexao_wifi(void);
void piscar(int tempo);
void Enviar_a_nuvem(void);
//void Processar_dados(void);
void Receber_dado(void);
 
/*
* Implementações
*/
 
/* Função: envia informações ao ThingSpeak
* Parâmetros: String com a informação a ser enviada
* Retorno: nenhum
*/
void envia_informacoes_thingspeak(String string_dados)
{
    if (client.connect(endereco_api_thingspeak, 80))
    {
        /* faz a requisição HTTP ao ThingSpeak */
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+chave_escrita_thingspeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(string_dados.length());
        client.print("\n\n");
        client.print(string_dados);
         
        last_connection_time = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
        piscar(900);
    }
}
 
/* Função: inicializa wi-fi
* Parametros: nenhum
* Retorno: nenhum
*/
void init_wifi(void)
{
    Serial.println("------WI-FI -----");
    Serial.println("Conectando-se a rede: ");
    Serial.println(SSID_REDE);
    Serial.println("\nAguarde...");
 
    conecta_wifi();
}
 
/* Função: conecta-se a rede wi-fi
* Parametros: nenhum
* Retorno: nenhum
*/
void conecta_wifi(void)
{
    /* Se ja estiver conectado, nada é feito. */
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
     
    /* refaz a conexão */
    WiFi.begin(SSID_REDE, SENHA_REDE);
     
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        digitalWrite(LED_PLACA, LOW);
    }
 
    Serial.println("Conectado com sucesso a rede wi-fi \n");
    Serial.println(SSID_REDE);
    digitalWrite(LED_PLACA, HIGH);
    piscar(500);
    piscar(500);
}
 
/* Função: verifica se a conexao wi-fi está ativa
* (e, em caso negativo, refaz a conexao)
* Parametros: nenhum
* Retorno: nenhum
*/
void verifica_conexao_wifi(void)
{
    conecta_wifi();
}

/* Função para piscar o led cada vez que a informação é enviada
 *  
 *  
 */
 void piscar(int tempo)
 {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(tempo);
  digitalWrite(LED_BUILTIN, LOW);
  delay(tempo);
 }

 float ler_temperatura(void)
 {
  int valorObtido = analogRead(A0);
  float milivolts = (valorObtido/1024.0) * 3300; 
  float celsius = milivolts/10;
  Serial.print("A temperatura é de = ");
  Serial.println(celsius);
  return(celsius);
 }
 
void setup()
{
    Serial.begin(9600);
    loraSerial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PLACA, OUTPUT);
    last_connection_time = 0;
 
    /* Inicializa sensor de temperatura e umidade relativa do ar */
 
    /* Inicializa e conecta-se ao wi-fi */
    init_wifi();
}
void Receber_dado()
{

int leitura = 0;
int conversoes = 0;


  // if there's any serial available, read it:
  while (loraSerial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    leitura = loraSerial.parseInt();
    if (leitura == 0) {
      Serial.println("achei um texto");
      conversoes = conversoes + 1;
    }
    else
    {
      conversoes = conversoes + 1;
    }
    if (conversoes == 1)
    {
    ID = leitura;
    }
    if (conversoes == 2)
    {  
    fumaca = leitura/100.0;
    }
    if (conversoes == 3)
    {
    temp = leitura/100.0;
    }
    if (conversoes == 4)
    {
    umidade = leitura/100.0;
    Serial.println(conversoes);
    Serial.println(umidade);
    }
  }
  conversoes = 0;
  Serial.print("O id é: ");
  Serial.print(ID);
  Serial.print(" A fumaça é:");
  Serial.print(fumaca);
  Serial.print(" A temperatura é:");
  Serial.print(temp);
  Serial.print(" A Umidade é:");
  Serial.print(umidade);
  Serial.println("esta ok");
  delay(1000);
}

 /*************************/
 
//loop principal
void loop()
{
    char fields_a_serem_enviados[100] = {0};
    float temperatura_lida = 0.0;
    float umidade_lida = 0.0;
    Receber_dado();
    Processar_dados();
    Serial.print("A fumaça 3 é = ");
    Serial.println(fumaca3);
    /* Força desconexão ao ThingSpeak (se ainda estiver conectado) */
    if (client.connected())
    {
        client.stop();
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
    }
 
    /* Garante que a conexão wi-fi esteja ativa */
    verifica_conexao_wifi();
     
    /* Verifica se é o momento de enviar dados para o ThingSpeak */
    if( millis() - last_connection_time > INTERVALO_ENVIO_THINGSPEAK )
    {
        temperatura_lida = ler_temperatura();        
        //temperatura_lida = dht.readTemperature();
        //umidade_lida = dht.readHumidity();
        //sprintf(fields_a_serem_enviados,"field1=%.2f&field2=%.2f", temperatura_lida, temperatura_lida);
        sprintf(fields_a_serem_enviados,"field1=%.2f&field2=%.2f&field3=%.2f&field4=%.2f&field5=%.2f", fumaca1, fumaca2,fumaca3,fumaca4,fumaca5);
        Serial.println(fumaca3);
        envia_informacoes_thingspeak(fields_a_serem_enviados);
    }
 
    delay(1000);
}


void Processar_dados()
{
    if (ID == 1)
    {
      fumaca1 = fumaca;
    }
    if (ID == 2)
    {
      fumaca2 = fumaca;
    }
     if (ID == 3)
    {
      
      fumaca3 = fumaca;      
      
    }
     if (ID == 4)
    {
      fumaca4 = fumaca;
    }
     if (ID == 5)
    {
      fumaca5 = fumaca;
    }
    ID = 0;   //Direciona a proxima atualização a um id inexistente
    
}
