#define  PWM_OUT_1         9
#define PWM_OUT_2          10

void configurePwmTimer() {
  TCCR1B = (TCCR1B & 0xF8) | 0x02; ///sets the frequency to 3900Hz (0x01 - 32kHz, 0x02 - 3900Hz, 0x03 - 490Hz default, 0x04 - 120Hz, 0x05 - 30Hz)
}

void setup() {
  Serial.begin(115200);
  pinMode(PWM_OUT_1,OUTPUT);
  pinMode(PWM_OUT_2,OUTPUT);
  configurePwmTimer();
}

void loop() {

  //Verificado dado da porta serial
  if (Serial.available()) {

    //Lendo o byte mais recente
    unsigned char byteRead = Serial.read();

    if ((byteRead >= CH1_NULL_SPEED_BYTE) &&  (byteRead <= CH1_MAX_SPEED_BYTE)) {
      analogOut(PWM_OUT_1, (255 * (byteRead - CH1_NULL_SPEED_BYTE) / (CH1_MAX_SPEED_BYTE - CH1_NULL_SPEED_BYTE)))
    }

    if ((byteRead >= CH2_NULL_SPEED_BYTE) &&  (byteRead <= CH2_MAX_SPEED_BYTE)) {
      analogOut(PWM_OUT_2, (255 * (byteRead - CH2_NULL_SPEED_BYTE) / (CH2_MAX_SPEED_BYTE - CH2_NULL_SPEED_BYTE)));
    }

    break;
  }

}
