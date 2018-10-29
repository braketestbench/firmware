#define MCU_LED_1   15


void configureTimer4(){
  TCCR4A = 0;TCCR4B = 0;TCCR4C = 0;TCCR4D = 0;
  TCCR4B |= (1 << CS43)|(1 << CS42)|(1 << CS41)|(1 << CS40);
  TCNT4 = 0;
  OCR4A = 500; //16MHz/16384/2Hz=488 ~ 1.95Hz
  TIMSK4 |= (1 << OCIE4A); //Enable Interrupt on compare input 4
}

ISR(TIMER4_COMPA_vect){
  digitalWrite(MCU_LED_1, digitalRead(MCU_LED_1) ^ 1);   // toggle LED pin
}

void setup() {
  pinMode(MCU_LED_1,OUTPUT);
  digitalWrite(MCU_LED_1,LOW);
  configureTimer4();
}

void loop() {
  // put your main code here, to run repeatedly:

}
