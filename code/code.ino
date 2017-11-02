//Libraries
#include <TimerOne.h>
#include <MsTimer2.h>
#include <math.h>

//HW DEFINITIONS
//Digital ports
///inputs
#define	ADC_CON_0			2
#define	ADC_CON_1			3
#define	ADC_CON_2			4
#define	ADC_CON_3			6
#define	ADC_CON_4			7
#define	ADC_CON_5			8
///outputs
#define DIG_OUT_PORT_0		        9
#define	DIG_OUT_PORT_1		        10
#define RELAY_OUT_0			11
#define RELAY_OUT_1			12
#define	ACQ_LED				13

//Analog Ports
#define	PWM_OUT				5

//COMMUNICATIONS DEFINITIONS
#define BAUD_RATE			9600
#define START_ACQ_BYTE		        33
#define	STOP_ACQ_BYTE		        34
#define NULL_DIGITAL_BYTE	        96
#define MAX_DIGITAL_BYTE	        111
#define NULL_SPEED_BYTE		        155
#define MAX_SPEED_BYTE		        255
#define SENSOR_ERROR		        -1


//GENERAL DEFINITIONS
#define SET				1
#define RESET				0
#define ACQ_LED_BLINK_MS	        250
#define SAMPLE_RATE			10
#define NUMBER_OF_SAMPLES	        100

//GLOBAL VARS
int adcReads[]={
  RESET,RESET,RESET,RESET,RESET,RESET};
short DIGITAL_OUT_PORTS[]={
  DIG_OUT_PORT_0,DIG_OUT_PORT_1,RELAY_OUT_0,RELAY_OUT_1,ACQ_LED};
short DIGITAL_IN_PORTS[]={
  ADC_CON_0,ADC_CON_1,ADC_CON_2,ADC_CON_3,ADC_CON_4,ADC_CON_5};
int samples=RESET;
boolean state=RESET;


void digitalOutputControl(unsigned char input){
  for(int i=RESET;i<4;i++){
    digitalWrite(DIGITAL_OUT_PORTS[i],((input >> i) & 1));
  }
}

void resetDigitalOutputs(){
  digitalOutputControl(NULL_DIGITAL_BYTE);
  digitalWrite(ACQ_LED, RESET);
}

void IOsetup(){
  //inputs
  pinMode(ADC_CON_0,INPUT);
  pinMode(ADC_CON_1,INPUT);
  pinMode(ADC_CON_2,INPUT);
  pinMode(ADC_CON_3,INPUT);
  pinMode(ADC_CON_4,INPUT);
  pinMode(ADC_CON_5,INPUT); 

  //Outputs
  pinMode(ACQ_LED,OUTPUT);
  pinMode(DIG_OUT_PORT_0,OUTPUT);
  pinMode(DIG_OUT_PORT_1,OUTPUT);
  pinMode(RELAY_OUT_0,OUTPUT);
  pinMode(RELAY_OUT_1,OUTPUT);
  pinMode(PWM_OUT,OUTPUT);
  //Reset Outputs
  resetDigitalOutputs();

}

//Function to configure Timer 0 - PWM-OUT
void configureTimer0(){
  TCCR0B = TCCR0B & B11111000 | B00000001;
}

//Function to configure Timer 1 - ACQUISITION 
void configureTimer1(){
  Timer1.initialize(SAMPLE_RATE);
  Timer1.attachInterrupt(timer1_OISR);
  Timer1.stop();
}

//Function to configure timer 2 - LED
void configureTimer2() {
  MsTimer2::set(ACQ_LED_BLINK_MS, timer2_OISR); 
  MsTimer2::start();
}

//Timer 1 overflow interruption routine
void timer1_OISR(){
  if (samples < NUMBER_OF_SAMPLES){
    adcReads[0]+=analogRead(A0);
    adcReads[1]+=analogRead(A1);
    adcReads[2]+=analogRead(A2);
    adcReads[3]+=analogRead(A3);
    adcReads[4]+=analogRead(A4);
    adcReads[5]+=analogRead(A5);
    samples++;
  }
}


//Timer 2 overflow interruption routine
void timer2_OISR(){
  //Serial.print("teste\n");
  digitalWrite(ACQ_LED, digitalRead(ACQ_LED) ^ 1);
}



void clearAdcReads(){
  for(int i=RESET;i<sizeof(adcReads);i++){
    adcReads[i]=RESET;
  }
}

void acquisitionStart(){
  samples=RESET;
  clearAdcReads();
  resetDigitalOutputs();
  Timer1.restart();
  //MsTimer2::start();
  digitalWrite(ACQ_LED,HIGH);
}

void acquisitionStop(){
  Timer1.stop();
  //MsTimer2::stop();
  
  digitalWrite(ACQ_LED,LOW);
  samples=RESET;

  clearAdcReads();
  resetDigitalOutputs();

}

void setup(){
  Serial.begin(BAUD_RATE);
  IOsetup();
  configureTimer0();
  configureTimer1();
  //configureTimer2();
}

void loop(){
  unsigned char byteRead=RESET;

  if(Serial.available()){
    byteRead = Serial.read();
  }
  
  Serial.print(state);
  Serial.print("\n");
  
  switch(state) {

  case RESET  :
    if (byteRead==START_ACQ_BYTE){
      Serial.print("START ACQ\n");
      state=SET;
      acquisitionStart();
    }
    else{
      state=RESET;
    }
    break;

  case SET  :
    if (byteRead==STOP_ACQ_BYTE){
      state=RESET;
      Serial.print("STOP ACQ\n");

    }
    else{
      state=SET;
    }

    break;
  }
}






