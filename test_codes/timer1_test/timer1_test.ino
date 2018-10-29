#include <TimerOne.h>

#define MCU_LED_1   15

void configureTimer1(){
  Timer1.attachInterrupt(timer1_OISR);
  Timer1.initialize(1000000);
  Timer1.stop();
} 

void timer1_OISR(){
    digitalWrite(MCU_LED_1, digitalRead(MCU_LED_1) ^ 1);
}


void setup() {
  pinMode(MCU_LED_1,OUTPUT);
  digitalWrite(MCU_LED_1,LOW);
  configureTimer1();
  Timer1.restart();

}

void loop() {
  // put your main code here, to run repeatedly:

}
