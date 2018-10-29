#define MCU_LED_1   15

void configureAcquisitionTimer(){
  TCCR3A = 0;TCCR3B = 0;TCCR3C = 0;
  TCCR3B |= (1 << WGM32) |(1 << CS31)|(1 << CS30);
  TCNT3 = 0;
  OCR3A = 250; //16MHz/64/1000
  TIMSK3 |= (1 << OCIE3A); //Enable Interrupt on compare input 4
}

ISR(TIMER3_COMPA_vect){
  digitalWrite(MCU_LED_1, digitalRead(MCU_LED_1) ^ 1);   // toggle LED pin
}

void setup() {
  Serial.begin(115200);
  pinMode(MCU_LED_1,OUTPUT);
  digitalWrite(MCU_LED_1,LOW);
  configureAcquisitionTimer();
}

void loop() {
  // put your main code here, to run repeatedly:

}
