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
#define NULL_SPEED_BYTE		        120
#define MAX_SPEED_BYTE		        220
#define SENSOR_ERROR		        -1


//GENERAL DEFINITIONS
#define SET				1
#define RESET				0
#define ACQ_LED_BLINK_MS	        250
#define SAMPLE_RATE			1000
#define NUMBER_OF_SAMPLES	        100

//GLOBAL VARS
static boolean enableAcquisition=false;
static float adcReads[]={
  RESET,RESET,RESET,RESET,RESET,RESET};
short COMMAND_PORTS[]={
  DIG_OUT_PORT_0,DIG_OUT_PORT_1,RELAY_OUT_0,RELAY_OUT_1};
boolean commandInstruction[]={
  0,0,0,0};
short DIGITAL_IN_PORTS[]={
  ADC_CON_0,ADC_CON_1,ADC_CON_2,ADC_CON_3,ADC_CON_4,ADC_CON_5};
int samples=RESET;
boolean state=RESET;


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
      adcReads[0]+=analogRead(A0);
      adcReads[1]+=analogRead(A1);
      adcReads[2]+=analogRead(A2);
      adcReads[3]+=analogRead(A3);
      adcReads[4]+=analogRead(A4);
      adcReads[5]+=analogRead(A5);
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

//FUNCAO QUE CONTROLA OS COMANDOS/SAIDAS-DIGITAIS
void commandOutput(){
  for(int i=0;i<sizeof(COMMAND_PORTS);i++){
    if(commandInstruction[i]){
      digitalWrite(COMMAND_PORTS[i],HIGH);
    }
    else{
      digitalWrite(COMMAND_PORTS[i],LOW);
    }
  }


}

//Funcao que "reseta" os comandos/saidas digitais
void resetCommandOutput(){
  for(int i=0;i<sizeof(COMMAND_PORTS);i++){
    commandInstruction[i]=0;
  }
  commandOutput();
}

//Funcao quando a acquisicao para
void acquisitionStop(){
  Timer1.stop();
  enableAcquisition=false;
  samples=RESET;
  for(int i=0;i<6;i++){
    adcReads[i]=RESET;
  }
  resetCommandOutput();
}

//Funcao que escreve os resultados da aquisicao na porta serial
void printResults(int * result){
  int i=0;
  for(i=0;i<6;i++){
    if (!(digitalRead(DIGITAL_IN_PORTS[i]))){
      Serial.print(result[i]);
    }else{
      Serial.print(SENSOR_ERROR);
    }
    
    if(i<5){
      Serial.print(",");
    }else{
      Serial.print("\n");
    }
  }  
  
}

void speedControl(unsigned char input){
	float analogOut = 1023*(input - NULL_SPEED_BYTE)/(MAX_SPEED_BYTE - NULL_SPEED_BYTE);
        Serial.print(analogOut);
        Serial.print("\n");
	//analogWrite(PWM_OUT,analogOut);
}

void setup(){

  Serial.begin(9600);
  pinMode(ACQ_LED,OUTPUT);
  for(int i=0;i<4;i++){
    pinMode(COMMAND_PORTS[i],OUTPUT);
    digitalWrite(COMMAND_PORTS[i],LOW);
  }
  digitalWrite(ACQ_LED,LOW);

  configureTimer1();
  configureTimer2();
  MsTimer2::start();


}

//Funcao loop - executada continuamente
void loop(){
  //parar o timer quando o numero de amostras for o desejado
  if(samples>=NUMBER_OF_SAMPLES){
    Timer1.stop();

    static int resultAcq[6];
    for(int counter=0;counter<6;counter++){
      resultAcq[counter] = (int)roundf((adcReads[counter]/samples));    
      adcReads[counter]=RESET;
    }
    samples=RESET;
    Timer1.restart();   //reinciando o timer 

    printResults(resultAcq);

  }

  //Verificado dado da porta serial
  if (Serial.available()){

    //Lendo o byte mais recente
    unsigned char byteRead = Serial.read();

    switch(byteRead) {

      //Recebeu comando iniciar aquisicao
    case START_ACQ_BYTE:
      {
        if(enableAcquisition==false){
          //Habilita a aquisicao
          enableAcquisition=true;
          samples=RESET;
          Timer1.restart();       
        }            
      }
      break;
    case STOP_ACQ_BYTE:
      {
        acquisitionStop();
      }
      break;

    default:
      {
        if ((byteRead >= NULL_DIGITAL_BYTE) &&  (byteRead <= MAX_DIGITAL_BYTE)){       //Controle dos comandos
          for (int i = 3; i>-1; i--) {
            commandInstruction[i]=((byteRead >> i) & 1);
          }
          commandOutput();

        }
        if ((byteRead >= NULL_SPEED_BYTE) &&  (byteRead <= MAX_SPEED_BYTE)){       //Controle dos comandos
          speedControl(byteRead);
        }
      }

      break; 
    }
  }
}









