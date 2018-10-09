//Libraries
#include <TimerOne.h>
#include <MsTimer2.h>
#include <math.h>

//HW DEFINITIONS
//Digital Inputs
#define	CKP_CON_1					7
#define	THERM_CON_1					4
#define	THERM_CON_2					12
#define	LOAD_CON_1					5
#define	LOAD_CON_2					13
#define	VIB_CON_1					11
#define DIG_IN_1					1
#define DIG_IN_2					14
#define DIG_IN_3					16

//Digital Outputs
#define DIG_OUT_1		        	3
#define	DIG_OUT_2		        	2
#define	DIG_OUT_3					0
#define	MCU_LED_1					15

//Analog Inputs
#define CKP_SIG_1					A0
#define THERM_SIG_1					A1
#define THERM_SIG_2					A2
#define LOAD_SIG_1					A3
#define LOAD_SIG_2					A4
#define VIB_SIG_1					A5
#define AOUT_SIG_1					A7
#define AOUT_SIG_2					A8

//Analog Outputs
#define	PWM_OUT_1					9
#define	PWM_OUT_2					10				

//COMMUNICATIONS DEFINITIONS
#define BAUD_RATE					115200
#define FILTER_READ_BYTE		    33
#define	ONE_READ_BYTE		        34
#define CH1_NULL_SPEED_BYTE		    75
#define CH1_MAX_SPEED_BYTE		    100
#define CH2_NULL_SPEED_BYTE		    101
#define CH2_MAX_SPEED_BYTE		    126


//GENERAL DEFINITIONS
#define SET							1
#define RESET						0
#define ACQ_LED_BLINK_MS	        150
#define NUMBER_OF_SAMPLES	        100

//GLOBAL VARS
static boolean enableAcquisition=false;
static float adcReads[]={
  RESET,RESET,RESET,RESET,RESET,RESET,RESET,RESET};
int samples=RESET;
boolean state=RESET;
int inputPins[] = {CKP_CON_1,THERM_CON_1,THERM_CON_2,LOAD_CON_1,LOAD_CON_2,VIB_CON_1,DIG_IN_1,DIG_IN_2,DIG_IN_3};					16
int outputPins[] = {DIG_OUT_1,DIG_OUT_2,DIG_OUT_3,MCU_LED_1};

//Funcao que configura o timer 1 - Aquisicao
void configureTimer1(){
  Timer1.attachInterrupt(timer1_OISR);
  Timer1.initialize(SAMPLE_RATE);
  Timer1.stop();
}

//Funcao que configura o timer 2 - Led aquisicao
void configureTimer2() {
  MsTimer2::set(ACQ_LED_BLINK_MS, timer2_OISR); 
}

//Interrupcao pelo timer 1 por overflow
void timer1_OISR(){
  if (samples<NUMBER_OF_SAMPLES){
    if(enableAcquisition==true){
      //Lendo dado analogico
      adcReads[0]+=analogRead(CKP_SIG_1);
      adcReads[1]+=analogRead(THERM_SIG_1);
      adcReads[2]+=analogRead(THERM_SIG_2);
      adcReads[3]+=analogRead(LOAD_SIG_1);
      adcReads[4]+=analogRead(LOAD_SIG_2);
      adcReads[5]+=analogRead(VIB_SIG_1);
      adcReads[6]+=analogRead(AOUT_SIG_1);
      adcReads[7]+=analogRead(AOUT_SIG_2);
      samples++;
    }   
  }
}

//Interrupcao pelo timer 2 por overflow
void timer2_OISR(){
  if(enableAcquisition){
    digitalWrite(ACQ_LED, digitalRead(ACQ_LED) ^ 1);
  }
  else{
    digitalWrite(ACQ_LED, LOW);
  }
}


//Funcao que "reseta" os comandos/saidas digitais
void resetCommandOutput(){
	digitalWrite(DIG_OUT_1,HIGH);
	digitalWrite(DIG_OUT_2,HIGH);
	digitalWrite(DIG_OUT_3,HIGH);
}

//Funcao quando a acquisicao para
void acquisitionStop(){
  Timer1.stop();
  enableAcquisition=false;
  samples=RESET;
  for(int i=RESET;i<adcReads.length();i++){
    adcReads[i]=RESET;
  }
  resetCommandOutput();
}

//Funcao que escreve os resultados da aquisicao na porta serial
void printResults(int * result){
	int i,j;
	for(i=RESET;i<adcReads.length();i++){
   		Serial.print(result[i]);
	    Serial.print(",");
	}

	for(j=RESET;i<inputPins.length();j++){
		if(digitalRead(inputPins[j])){
			Serial.print("1");
		}else{
			Serial.print("0");
		}

		if(j<(inputPins.length() - 1)){
			Serial.print(",");
		}else{
			Serial.print("\n");
		}
	}
}

void oneReadPrint(){
	int read[] = {RESET,RESET,RESET,RESET,RESET,RESET,RESET,RESET};
	read[0]= AnalogRead(CKP_SIG_1);
	read[1]= AnalogRead(THERM_SIG_1);
	read[2]= AnalogRead(THERM_SIG_2);
	read[3]= AnalogRead(LOAD_SIG_1);
	read[4]= AnalogRead(LOAD_SIG_2);
	read[5]= AnalogRead(VIB_SIG_1);
	read[6]= AnalogRead(AOUT_SIG_1);
	read[7]= AnalogRead(AOUT_SIG_2);

	printResults(read);
}

void setup(){
	//Initializing Serial port
  	Serial.begin(BAUD_RATE);

  	//Port Map
	for(int i=RESET;i < inputPins.length();i++){
		pinMode(inputPins[i],INPUT);
	}
	for(i=RESET; i < outputPins.length();i++){
		pinMode(outputPins[i],OUTPUT);
	}
	resetCommandOutput();

	//Configuring Timers
	configureTimer1();
	configureTimer2();
	MsTimer2::restart();
}

//Funcao loop - executada continuamente
void loop(){
  //parar o timer quando o numero de amostras for o desejado
  if(samples>=NUMBER_OF_SAMPLES){
    Timer1.stop();
    enableAcquisition=false;

    static int resultAcq[adcReads.length()];
    for(int counter=RESET;counter<adcReads.length();counter++){
      resultAcq[counter] = (int)roundf((adcReads[counter]/samples));    
      adcReads[counter]=RESET;
    }
    samples=RESET;
    adcReads[]={
  RESET,RESET,RESET,RESET,RESET,RESET,RESET,RESET};
    printResults(resultAcq);
  }

  //Verificado dado da porta serial
  if (Serial.available()){

    //Lendo o byte mais recente
    unsigned char byteRead = Serial.read();

    switch(byteRead) {

      //Recebeu comando iniciar aquisicao
    case FILTER_READ_BYTE:
      {
        if(enableAcquisition==false){
          //Habilita a aquisicao
          enableAcquisition=true;
          samples=RESET;
          Timer1.restart();       
        }            
      }
      break;
    case ONE_READ_BYTE:
      {
        oneReadPrint();
      }
      break;

    default:
      {
        if ((byteRead >= NULL_DIGITAL_BYTE) &&  (byteRead <= MAX_DIGITAL_BYTE)){       //Controle dos comandos
          unsigned char command = byteRead - NULL_DIGITAL_BYTE;

          switch(command){
          	case 36:
          		digitalWrite(DIG_OUT_1,LOW);
          	break;
          		
          	case 37:
          		digitalWrite(DIG_OUT_1,HIGH);
          	break;
          		
          	case 38:
          		digitalWrite(DIG_OUT_2,LOW);
          	break;
          			
          	case 39:
          		digitalWrite(DIG_OUT_2,HIGH);
          	break;
          		
          	case 40:
          		digitalWrite(DIG_OUT_3,LOW);
          	break;

          	case 41:
          		digitalWrite(DIG_OUT_3,HIGH);
          	break;

          	default:
          		resetCommandOutput();
          	break;
          	
          }

        }
        if ((byteRead >= CH1_NULL_SPEED_BYTE) &&  (byteRead <= CH1_MAX_SPEED_BYTE)){
        	analogOut(PWM_OUT_1,(255*(byteRead - CH1_NULL_SPEED_BYTE)/(CH1_MAX_SPEED_BYTE - CH1_NULL_SPEED_BYTE)))
        }

        if ((byteRead >= CH2_NULL_SPEED_BYTE) &&  (byteRead <= CH2_MAX_SPEED_BYTE)){
        	analogOut(PWM_OUT_2,(255*(byteRead - CH2_NULL_SPEED_BYTE)/(CH2_MAX_SPEED_BYTE - CH2_NULL_SPEED_BYTE)));
        }

      break; 
    }
  }
}