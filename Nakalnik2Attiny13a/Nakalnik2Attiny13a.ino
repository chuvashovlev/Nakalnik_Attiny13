#define TLED_CHIP LED_WS2812
#define TLED_ORDER ORDER_GRB
#define TLED_STATIC_BRIGHT

#include <EEPROM.h>
#include "tinyLED.h"
tinyLED<1> strip;

#define DELAY_ON 150
int btnTmr1=0, btnTmr2=0, volts, ledTmr1, ledTmr2;
long logicTmr, PWMtmr, tmr1;
bool f=0, fb1=0, fb2=0, ledflag1=0, ledflag2=0;
byte filling;

void Buttons(){
  uint16_t value = analogRead(A2);
  if(value>615){
    if(!fb1 and millis()-btnTmr1>100){
      fb1=1;
      btnTmr1=millis();
      if(filling<255)
        filling++;
    }
  }
  else if(value>368){
    if(!fb2 and millis()-btnTmr2>100){
      fb2=1;
      btnTmr2=millis();
      if(filling>0)
        filling--;
    }
  }
  if(value<300 and (fb1 or fb2) and (millis()-btnTmr1>100 or millis()-btnTmr2>100)){
    fb1=0;
    fb2=0;
    btnTmr1=millis();
    btnTmr2=millis();
  }
}

void Indicator(){
  //analogWrite(PB0, 255);
  if(analogRead(A3)>10){
    f=1;
    tmr1=millis();
    }
  if(millis()-tmr1>=DELAY_ON){
      tmr1=millis();
      f=0;
  }
  //analogWrite(PB0, filling);
}

void setup() {
  pinMode(PB0, OUTPUT);
  filling = EEPROM.read(1);
  /*TCCR0A=0b10000011;
  TCCR0B=0b00000001;
  OCR0A=filling;*/
}

void loop() {
  Buttons();
  Indicator();
  volts = analogRead(A1);//*0.0048828125*3.13;

  if(millis()-logicTmr>=5){
    logicTmr=millis();
    if(f and volts>655){
      strip.send(mGreen);
      ledTmr1=millis();
      ledTmr2=millis();
    }
    else if(f and volts<655){
      if(millis()-ledTmr1>=500){
        ledTmr1=millis();
        ledflag1=!ledflag1;
      }
      if(ledflag1){
        strip.send(mGreen);
      }
      else{
        strip.send(mYellow);
      }
    }
    else if(!f and volts>655){
      strip.send(mRed);
      ledTmr1=millis();
      ledTmr2=millis();
    }
    else if(!f and volts<655){
      if(millis()-ledTmr2>=500){
        ledTmr2=millis();
        ledflag2=!ledflag2;
      }
      if(ledflag2){
        strip.send(mRed);
      }
      else{
        strip.send(mYellow);
      }
    }
  }
  analogWrite(PB0, filling);
  //OCR0A=filling;
  EEPROM.update(1, filling);
}
