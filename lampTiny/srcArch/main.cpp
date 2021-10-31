// This is the WS2811 version with PWM controll and button controll
// © Dmytro Luchyn 05.05.2021


#include <Arduino.h>
#include "GyverButton.h"
#include <microLED.h>

#define COLOR_DEBTH 3
#define STRIP_PIN 1
#define NUMLEDS 5
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;

#define LED1 2
#define LED2 1
#define LED3 0 //PWM controll
#define mosfetPin 4
#define voltagePin A3


const float cutVoltage = 14.33;
const float maxVoltage = 20.33;
boolean permaOff = false;
boolean startComlete = false;
unsigned long millisTimer = 0;
byte mode = 0;

GButton button(LED1);
void setup() {
  analogReference(INTERNAL); // voltage reference = 1.1 volt
  pinMode(voltagePin, INPUT);

  strip.setBrightness(70);
  button.setTimeout(600);
  button.setClickTimeout(200);
  pinMode(mosfetPin, OUTPUT);
  strip.show();
}
float getVoltage(){
  const unsigned int R1 = 12080; //(ohm)
  const unsigned int R2 = 471; //(ohm)
  float sourceVoltage = 0.0;
  const float Vref = 1.06;
  float ADC_Voltage = 0.0;
  ADC_Voltage = analogRead(voltagePin)*(Vref/1023);  
  sourceVoltage = ((R1+R2)*ADC_Voltage)/R2;
  return sourceVoltage;
}
void smoothStart()
{
  mode = 3;
  static byte i = 255;
  i--;
  analogWrite(LED3, i);
  strip.set(round(i/(float)(255/4)), mRGB(66, 215, 245));
  strip.show();

  if (i == 20)
  {
    startComlete = true;
    i = 255;
  }
}
void BMS(){ 
  float currentVoltage = getVoltage();
  if(startComlete){
  if(currentVoltage < cutVoltage){
    //0%
    permaOff = true;
    strip.set(0, mBlack);
    strip.set(1, mBlack);
    strip.set(2, mBlack);
    strip.set(3, mBlack);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)*3){
    //75%-100%
    strip.set(0, mRed);
    strip.set(1, mOrange);
    strip.set(2, mYellow);
    strip.set(3, mGreen);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)*2){
    //50%-75%
    strip.set(0, mRed);
    strip.set(1, mOrange);
    strip.set(2, mYellow);
    strip.set(3, mBlack);
  }else if(currentVoltage > cutVoltage + ((maxVoltage-cutVoltage)/4)){
    //25%-50%
    strip.set(0, mRed);
    strip.set(1, mOrange);
    strip.set(2, mBlack);
    strip.set(3, mBlack);
  }else if(currentVoltage < cutVoltage + ((maxVoltage-cutVoltage)/4) && currentVoltage > cutVoltage){
    //~25%
    strip.set(0, mRed);
    strip.set(1, mBlack);
    strip.set(2, mBlack);
    strip.set(3, mBlack);
  }
  strip.show();
  }
  if(!permaOff){
    digitalWrite(mosfetPin, HIGH);
  }else{
    digitalWrite(mosfetPin, LOW);
  }
}
void setBrightness()
{
  if (startComlete)
  {
    switch (mode)
    {
    case 0:
      analogWrite(LED3, 0); //100% - 27757 lux
      strip.set(4, mWhite);
      break;
    case 1:
      analogWrite(LED3, 10); //75% - 10917 lux 20000 traget
      strip.set(4, mBlue);
      break;
    case 2:
      analogWrite(LED3, 15); //50% - 2576 lux
      strip.set(4, mYellow);
      break;
    case 3:
      analogWrite(LED3, 20); //25% - 280 lux
      strip.set(4, mOrange);
      break;
    case 4:
    analogWrite(LED3, 200); //5%
      strip.set(4, mRed);
      break;
    default:
      analogWrite(LED3, 0);
      strip.set(4, mWhite);
      break;
    }
    strip.show();
  }
}
void modeSelector()
{
  if (startComlete)
  {
    if (button.isSingle() && mode <= 4)
    {
      mode++;
    }
    else if (mode > 4)
    {
      mode = 0;
    }
  }
}
void loop() {
  if (!startComlete && millis() - millisTimer >= 10)
  {
    smoothStart();
    millisTimer = millis();
  }
  setBrightness();
  modeSelector();
  BMS();

  button.tick();
}



// #include <Arduino.h>
// #include <SoftwareSerial.h>
// float getVoltage(){
//   const unsigned int R1 = 12080; //(ohm)
//   const unsigned int R2 = 471; //(ohm)
//   float sourceVoltage = 0.0;
//   const float Vref = 1.06;
//   float ADC_Voltage = 0.0;
//   ADC_Voltage = analogRead(A3)*(Vref/1023);
  
//   sourceVoltage = ((R1+R2)*ADC_Voltage)/R2;
//   return sourceVoltage;
// }
// SoftwareSerial mySerial(2, 1); // RX, TX
// void setup(){
//   analogReference(INTERNAL);
//   pinMode(A3, INPUT);
//   mySerial.begin(9600);
// }
// void loop(){
//   mySerial.print(analogRead(A3));
//   mySerial.print("-");
//   mySerial.println(getVoltage());
// }