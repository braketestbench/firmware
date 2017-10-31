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
#define DIG_OUT_PORT_0		9
#define	DIG_OUT_PORT_1		10
#define RELAY_OUT_0			11
#define RELAY_OUT_1			12
#define	ACQ_LED				13

//Analog Ports
#define	PWM_OUT				5

//COMMUNICATIONS DEFINITIONS
#define BAUD_RATE			9600
#define START_ACQ_BYTE		21
#define	STOP_ACQ_BYTE		22
#define NULL_DIGITAL_BYTE	96
#define MAX_DIGITAL_BYTE	111
#define NULL_SPEED_BYTE		155
#define MAX_SPEED_BYTE		255
#define SENSOR_ERROR		-1


//GENERAL DEFINITIONS
#define SET					1
#define RESET				0
#define ACQ_LED_BLINK_MS	250
#define SAMPLE_RATE			10
#define NUMBER_OF_SAMPLES	100

//GLOBAL VARS
static boolean acquisitionEnabled=false;
int samples=RESET;
boolean state=RESET;
int adcReads[]={RESET,RESET,RESET,RESET,RESET,RESET};
short DIGITAL_OUT_PORTS[]={DIG_OUT_PORT_0,DIG_OUT_PORT_1,RELAY_OUT_0,RELAY_OUT_1};
short DIGITAL_IN_PORTS[]={ADC_CON_0,ADC_CON_1,ADC_CON_2,ADC_CON_3,ADC_CON_4,ADC_CON_5};

//Function to configure Timer 0 - PWM-OUT
void configureTimer0(){
	TCCR0B = (TCCR0B & 0b11111000) | 0x01;
}

void IOsetup(){
	pinMode(ACQ_LED,OUTPUT);
	
	for(int i=RESET;i<sizeof(DIGITAL_IN_PORTS);i++){
		if (i<sizeof(DIGITAL_OUT_PORTS)){
			pinMode(DIGITAL_OUT_PORTS[i],OUTPUT);
		}
		pinMode(DIGITAL_IN_PORTS[i],INPUT);
	}
	
	resetDigitalOutputs();
}

void digitalOutputControl(char input){
	for(int i=RESET;i<sizeof(DIGITAL_OUT_PORTS[i]);i++){
		digitalWrite(DIGITAL_OUT_PORTS[i],((input >> i) & 1));
	}
}

void resetDigitalOutputs(){
	digitalOutputControl(NULL_DIGITAL_BYTE);
	digitalWrite(ACQ_LED, LOW);
}

//Function to configure Timer 1 - ACQUISITION 
void configureTimer1(){
	Timer1.attachInterrupt(timer1_OISR);
	Timer1.initialize(SAMPLE_RATE);
	Timer1.stop();
}

//Function to configure timer 2 - LED
void configureTimer2() {
	MsTimer2::set(ACQ_LED_BLINK_MS, timer2_OISR); 
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
	digitalWrite(ACQ_LED, digitalRead(ACQ_LED) ^ 1);
}

void avgPrintResults(){
	
	for(int i=RESET;i<sizeof(adcReads);i++){
		if (!digitalRead(DIGITAL_IN_PORTS[i])){
			Serial.print((int)roundf((adcReads[i]/samples)));
		}else{
			Serial.print(SENSOR_ERROR);
		}
		Serial.print(",");
		adcReads[i]=RESET;
	}
	Serial.println();
	samples=RESET;
	Timer1.restart();
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
    MsTimer2::start();
}

void acquisitionStop(){
	Timer1.stop();
	MsTimer2::stop();
	samples=RESET;
	
	clearAdcReads();
	resetDigitalOutputs();
	
}

void speedControl(char speed_byte){
	speed_byte-=NULL_SPEED_BYTE;
	int speed = 1023*speed_byte/(MAX_SPEED_BYTE - NULL_SPEED_BYTE);
	analogWrite(PWM_OUT,speed);
}

void setup(){
	Serial.begin(BAUD_RATE);
	IOsetup();
	configureTimer0();
	configureTimer1();
	configureTimer2();
}

void loop(){
	boolean nextState;
	char byteRead;
	
	if(Serial.available()){
		byteRead = Serial.read();
	}
	
	switch (state){
		case RESET:{
			if (byteRead=START_ACQ_BYTE){
					acquisitionStart();
					nextState=SET;
					
			}else{
					nextState=RESET;
			}
		}
		break;
		
		case SET:{
			if (samples>=NUMBER_OF_SAMPLES){
				avgPrintResults();
			}
			
			if (byteRead==STOP_ACQ_BYTE){
				acquisitionStop();
				nextState=RESET;
			}else{
				if (byteRead>=NULL_SPEED_BYTE){
					speedControl(byteRead);
				}else if ((byteRead>=NULL_DIGITAL_BYTE) && (byteRead<=MAX_DIGITAL_BYTE)){
					digitalOutputControl(byteRead);
				}
				nextState=SET;
			}
		}
		break;
		
	}
	state=nextState;
}
