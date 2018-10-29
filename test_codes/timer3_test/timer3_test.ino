#include <TimerThree.h>

#define MCU_LED_1   15

void configureTimer3(){
  Timer3.attachInterrupt(timer3_OISR);
  Timer3.initialize(200000);
  Timer3.stop();
} 

void timer3_OISR(){
    digitalWrite(MCU_LED_1, digitalRead(MCU_LED_1) ^ 1);
}


void setup() {
  pinMode(MCU_LED_1,OUTPUT);
  digitalWrite(MCU_LED_1,LOW);
  configureTimer3();
  Timer3.restart();

}

void loop() {
  // put your main code here, to run repeatedly:

}
