//HW DEFINITIONS
//Digital Inputs
#define	CKP_CON_1               7
#define	THERM_CON_1					    4
#define	THERM_CON_2					    12
#define	LOAD_CON_1					    5
#define	LOAD_CON_2					    13
#define	VIB_CON_1 					    11
#define DIG_IN_1  			    		1
#define DIG_IN_2   	    				14
#define DIG_IN_3      					16

//Digital Outputs
#define DIG_OUT_1		            3
#define	DIG_OUT_2		           	2
#define	DIG_OUT_3		    			  0
#define	MCU_LED_1   					  15

//Analog Inputs
#define CKP_SIG_1 					    A0
#define THERM_SIG_1					    A1
#define THERM_SIG_2					    A2
#define LOAD_SIG_1					    A3
#define LOAD_SIG_2					    A4
#define VIB_SIG_1					      A5
#define AOUT_SIG_1	  	  			A7
#define AOUT_SIG_2  	  				A8

//Analog Outputs
#define	PWM_OUT_1					      9
#define	PWM_OUT_2					      10

//COMMUNICATIONS DEFINITIONS
#define BAUD_RATE					      115200
#define PRINT_VERSION_BYTE      32
#define FILTER_READ_BYTE		    33
#define	ONE_READ_BYTE		        34
#define ACQ_CMD_OFF_BYTE        35
#define NULL_DIGITAL_BYTE       36
#define MAX_DIGITAL_BYTE        42
#define CH1_NULL_SPEED_BYTE		  75
#define CH1_MAX_SPEED_BYTE		  100
#define CH2_NULL_SPEED_BYTE		  101
#define CH2_MAX_SPEED_BYTE		  126

//GENERAL DEFINITIONS
#define SET							        1
#define RESET						        0
#define ACQ_LED_BLINK_MS	      150
#define NUMBER_OF_SAMPLES	      100
#define SAMPLE_RATE             250

//TIMER DEFINITIONS
#define MCU_LED_BLINK_FREQ 			2
#define SAMPLE_RATE_HZ		  		10000

//GLOBAL VARS
boolean enableAcquisition = false;
boolean enableMcuLed = false;
float adcReads[] = {RESET, RESET, RESET, RESET, RESET, RESET, RESET, RESET};
int samples = RESET;
boolean state = RESET;
int inputPins[] = {CKP_CON_1, THERM_CON_1, THERM_CON_2, LOAD_CON_1, LOAD_CON_2, VIB_CON_1, DIG_IN_1, DIG_IN_2, DIG_IN_3};
int outputPins[] = {DIG_OUT_1, DIG_OUT_2, DIG_OUT_3, MCU_LED_1};

//////TIMER CONFIGURING FUNCTIONS
//Function that sets Timer 1 - PWM frequency
void configurePwmTimer() {
  TCCR1B = (TCCR1B & 0xF8) | 0x02; ///sets the frequency to 3900Hz (0x01 - 32kHz, 0x02 - 3900Hz, 0x03 - 490Hz default, 0x04 - 120Hz, 0x05 - 30Hz)
}

//Function that sets Timer 3 - Acquisition Timer
void configureAcquisitionTimer() {
  TCCR3A = 0; TCCR3B = 0; TCCR3C = 0; TCNT3 = 0; //clearing timer registers
  TCCR3B |= (1 << WGM32) | (1 << CS31) | (1 << CS30);//setting up the clock divider (clk/1);
  OCR3A = 16000000 / 64 / SAMPLE_RATE_HZ; //16MHz/64/1000kHZ //Configuring the compare value, sample rate)
  TIMSK3 |= (1 << OCIE3A); //Enable Interrupt on compare input 3A
}

//Function that sets Timer 4 - MCU LED timer
void configureMcuLedTimer() {
  TCCR4A = 0; TCCR4B = 0; TCCR4C = 0; TCCR4D = 0; TCNT4 = 0; //clearing timer registers
  TCCR4B |= (1 << CS43) | (1 << CS42) | (1 << CS41) | (1 << CS40); //setting up the clock divider (clk/16384);
  OCR4A = 488 ;//16000 / 16384 / MCU_LED_BLINK_FREQ; //16MHz/16384/2Hz=488 ~ 1.95Hz
  TIMSK4 |= (1 << OCIE4A); //Enable Interrupt on compare input 4A
}

//Timer 3 Interupt Service Routine - Acquisition
ISR(TIMER3_COMPA_vect) {
  if (enableAcquisition) {
    if (samples <= NUMBER_OF_SAMPLES) {
      //Reading Analog Inputs
      adcReads[0] += analogRead(CKP_SIG_1);
      adcReads[1] += analogRead(THERM_SIG_1);
      adcReads[2] += analogRead(THERM_SIG_2);
      adcReads[3] += analogRead(LOAD_SIG_1);
      adcReads[4] += analogRead(LOAD_SIG_2);
      adcReads[5] += analogRead(VIB_SIG_1);
      adcReads[6] += analogRead(AOUT_SIG_1);
      adcReads[7] += analogRead(AOUT_SIG_2);
      samples++;
    }
  }
}

//Timer 4 Interupt Service Routine - Acquisition
ISR(TIMER4_COMPA_vect) {
  if (enableMcuLed) {
    digitalWrite(MCU_LED_1, digitalRead(MCU_LED_1) ^ 1); // toggle MCU Led Pin
  } else {
    digitalWrite(MCU_LED_1, LOW); //Turn MCU LED on
  }
}

//Function that turns all the digital outputs off
void resetCommandOutput() {
  digitalWrite(DIG_OUT_1, HIGH);
  digitalWrite(DIG_OUT_2, HIGH);
  digitalWrite(DIG_OUT_3, HIGH);
  analogWrite(PWM_OUT_1, RESET);
  analogWrite(PWM_OUT_2, RESET);
}

//Function that prints acquisition results in serial port
void printResults(int * result) {  
  for (int i = RESET; i < 8; i++) {
    Serial.print(result[i]);
    Serial.print(",");
  }

  for (int i = RESET; i < 3; i++) {
    if (digitalRead(inputPins[i])) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    if (i<2){
      Serial.print(",");
    }else{
      Serial.println(";");
    }
  }
}

void oneReadPrint() {
  int oneRead[] = {RESET, RESET, RESET, RESET, RESET, RESET, RESET, RESET};
  oneRead[0] = analogRead(CKP_SIG_1);
  oneRead[1] = analogRead(THERM_SIG_1);
  oneRead[2] = analogRead(THERM_SIG_2);
  oneRead[3] = analogRead(LOAD_SIG_1);
  oneRead[4] = analogRead(LOAD_SIG_2);
  oneRead[5] = analogRead(VIB_SIG_1);
  oneRead[6] = analogRead(AOUT_SIG_1);
  oneRead[7] = analogRead(AOUT_SIG_2);

  printResults(oneRead);
}

void printFwVersion(){
  Serial.println("");
  Serial.println("Braketestbench - FW Version 2.0 (28-Oct/2018)");
  Serial.println("Made by Joao Guimaraes - joaoguimaraes31@gmail.com");
  Serial.println("https://github.com/braketestbench/firmware");
  Serial.println("");
}

void setup() {
  //Initializing Serial port
  Serial.begin(BAUD_RATE);
  
  //Port Map
  for (int i = RESET; i < sizeof(inputPins); i++) {
    pinMode(inputPins[i], INPUT);
  }
  for (int i = RESET; i < sizeof(outputPins); i++) {
    pinMode(outputPins[i], OUTPUT);
  }
  resetCommandOutput();

  //Configuring Timers
  configurePwmTimer();
  configureAcquisitionTimer();
  configureMcuLedTimer();

  //statusLED
  enableMcuLed = true;

  //printFwVersion
  printFwVersion();
}

//Funcao loop - executada continuamente
void loop() {
  //parar o timer quando o numero de amostras for o desejado
  if (samples >= NUMBER_OF_SAMPLES) {
    enableAcquisition = false;
    enableMcuLed = true;
    TCNT3 = 0; //clearing timer count register
    static int resultAcq[sizeof(adcReads)];
    for (int counter = RESET; counter < 8; counter++) {
      resultAcq[counter] = (int)roundf((adcReads[counter] / samples));
      adcReads[counter] = RESET;
    }
    samples = RESET;
    printResults(resultAcq);
  }

  //Verificado dado da porta serial
  if (Serial.available()) {

    //Lendo o byte mais recente
    unsigned char byteRead = Serial.read();

    switch (byteRead) {

      //Recebeu comando iniciar aquisicao
      case FILTER_READ_BYTE:
        {
          if (!enableAcquisition) {
            TCNT3 = 0; //clearing timer count register
            enableAcquisition = true;
            enableMcuLed = false;
          }
        }
        break;
      case ONE_READ_BYTE:
        {
          if (!enableAcquisition) {
            oneReadPrint();
          }
        }
        break;
        
      case ACQ_CMD_OFF_BYTE:
        {
          resetCommandOutput();
          enableAcquisition = false;
        }
        break;
        
      case PRINT_VERSION_BYTE:
      {
        printFwVersion();
      }
      break;
        
      default:
        {
          if ((byteRead >= NULL_DIGITAL_BYTE) &&  (byteRead <= MAX_DIGITAL_BYTE)) {      //Controle dos comandos
            unsigned char command = byteRead;
            switch (command) {
              case 36:
                digitalWrite(DIG_OUT_1, LOW);
                break;

              case 37:
                digitalWrite(DIG_OUT_1, HIGH);
                break;

              case 38:
                digitalWrite(DIG_OUT_2, LOW);
                break;

              case 39:
                digitalWrite(DIG_OUT_2, HIGH);
                break;

              case 40:
                digitalWrite(DIG_OUT_3, LOW);
                break;

              case 41:
                digitalWrite(DIG_OUT_3, HIGH);
                break;

              default:
                digitalWrite(DIG_OUT_1, HIGH);
                digitalWrite(DIG_OUT_2, HIGH);
                digitalWrite(DIG_OUT_3, HIGH);
                break;

            }

          }
          if ((byteRead >= CH1_NULL_SPEED_BYTE) &&  (byteRead <= CH1_MAX_SPEED_BYTE)) {
            analogWrite(PWM_OUT_1, (255 * (byteRead - CH1_NULL_SPEED_BYTE) / (CH1_MAX_SPEED_BYTE - CH1_NULL_SPEED_BYTE)));
          }

          if ((byteRead >= CH2_NULL_SPEED_BYTE) &&  (byteRead <= CH2_MAX_SPEED_BYTE)) {
            analogWrite(PWM_OUT_2, (255 * (byteRead - CH2_NULL_SPEED_BYTE) / (CH2_MAX_SPEED_BYTE - CH2_NULL_SPEED_BYTE)));
          }

          break;
        }
    }
  }
}
