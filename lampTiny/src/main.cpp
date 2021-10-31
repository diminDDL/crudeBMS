#include <Arduino.h>

#define LED1 2
#define LED2 1
#define LED3 0
#define mosfetPin 4
#define voltagePin A3


const float cutVoltage = 14.33;
const float maxVoltage = 19.33;
boolean permaOff = false;
unsigned long millisTimer = 0;

float getVoltage(){
  const unsigned int R1 = 12050; //(ohm)
  const unsigned int R2 = 471; //(ohm)
  float sourceVoltage = 0.0;
  const float Vref = 1.1;
  float ADC_Voltage = 0.0;
  ADC_Voltage = analogRead(voltagePin)*(Vref/1023);  
  sourceVoltage = ((R1+R2)*ADC_Voltage)/R2;
  return sourceVoltage;
}
void BMS(){ 
  float currentVoltage = getVoltage();
  static bool blinker = false;
  if(currentVoltage < cutVoltage){
    //0%
    permaOff = true;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)*3){
    //75%-100%
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)*2){
    //50%-75%
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)){
    //25%-50%
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }else if(currentVoltage < cutVoltage + ((maxVoltage-cutVoltage)/4) && currentVoltage > cutVoltage){
    //~25%
    blinker = !blinker;
    digitalWrite(LED1, blinker);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
  }
  if(!permaOff){
    digitalWrite(mosfetPin, HIGH);
  }else{
    digitalWrite(mosfetPin, LOW);
  }
}

void setup(){
    analogReference(INTERNAL); // voltage reference = 1.1 volt
    pinMode(voltagePin, INPUT);
    pinMode(mosfetPin, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    millisTimer = millis();
    BMS();
    permaOff = false;
}
void loop(){
if(millis() - millisTimer > 1000){
    BMS();
    millisTimer = millis();
}
}