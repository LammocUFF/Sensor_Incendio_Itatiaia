//Receptor Itatiaia Smples


#include <SoftwareSerial.h>
 
#define LED1  LED_BUILTIN  
 
SoftwareSerial loraSerial(10, 11); // TX, RX  Arduino mega usar pinos 10, 11.
 
void setup() {
  pinMode(LED1, OUTPUT);
  Serial.begin(9600);
  loraSerial.begin(9600);  
}
 
void loop() { 
  if(loraSerial.available() > 1){
    String input = loraSerial.readString();
    Serial.println("Chegou!");  
    Serial.println(input);  
    if(input == "on") {
      digitalWrite(LED1, HIGH);  
    } 
    if(input == "off") {
      digitalWrite(LED1, LOW);
    }
  }
  else
  {
    Serial.println("..");
    delay(200);
  }
  delay(20);
}
