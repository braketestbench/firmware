//Bibliotecas
#include <TimerOne.h>
#include <MsTimer2.h>
#include <math.h>


//Definicoes Basicas para execucao
#define RESET                 0
#define ACQ_LED_PIN           13
#define START_ACQ             '('
#define STOP_ACQ              ')'
#define MASTER_RESET          '+'
#define ACQ_LED_BLINK_MS      250

//Definicao de Portas usadas para acionamento de comandos (saidas digitais). Portas 0 e 1 NUNCA podem ser usadas.
#define PORT0                2
#define PORT1                3
#define PORT2                4
#define PORT3                5

//Periods para serem escolhidos para definir a Sampling Rate
#define SAMPLE_PERIOD_0               1000000       //1Hz
#define SAMPLE_PERIOD_1               100000        //10Hz
#define SAMPLE_PERIOD_2               10000         //100Hz
#define SAMPLE_PERIOD_3               1000          //1kHz
#define SAMPLE_PERIOD_4               100           //10kHz
#define SAMPLE_PERIOD_5               10            //100kHz
#define DEFAULT_SAMPLE_PERIOD         1000          //1kHz      //Se nenhuma opcao for definida pelo usuario programa toma esse valor como padrao

//Opcoes para NumeroDeAmostras a serem consideradas no buffer de aquisicao
#define NUMBER_OF_SAMPLES_0           1
#define NUMBER_OF_SAMPLES_1           10
#define NUMBER_OF_SAMPLES_2           100
#define NUMBER_OF_SAMPLES_3           500
#define NUMBER_OF_SAMPLES_4           1000
#define NUMBER_OF_SAMPLES_5           10000
#define DEFAULT_NUMBER_OF_SAMPLES     100           //Se nenhuma opcao for definida pelo usuario programa toma esse valor como padrao

//Flag usada para definir se o programa esta em modo de execucao normal ou modo debug
//#define DEBUG                 false

//Variaveis Globais
static boolean enableAcquisition=false;
int samples=0;
short COMMAND_PORTS[]={PORT0,PORT1,PORT2,PORT3};
boolean commandInstruction[]={0,0,0,0};
unsigned long int RATES[]={SAMPLE_PERIOD_0,SAMPLE_PERIOD_1,SAMPLE_PERIOD_2,SAMPLE_PERIOD_3,SAMPLE_PERIOD_4,SAMPLE_PERIOD_5};
unsigned long int numberSamples = DEFAULT_NUMBER_OF_SAMPLES;
unsigned long int SAMPLES[]={NUMBER_OF_SAMPLES_0,NUMBER_OF_SAMPLES_1,NUMBER_OF_SAMPLES_2,NUMBER_OF_SAMPLES_3,NUMBER_OF_SAMPLES_4,NUMBER_OF_SAMPLES_5};
static float sensorData[]={0,0,0,0,0,0};
unsigned long int sampleRate=DEFAULT_SAMPLE_PERIOD;

//Funcao que configura o timer 1 - Aquisicao
void configureTimer1(){
  Timer1.attachInterrupt(timer1_OISR);
  Timer1.initialize(sampleRate);
  Timer1.stop();
}

//Funcao que configura o timer 2 - Led aquisicao
void configureTimer2() {
  MsTimer2::set(ACQ_LED_BLINK_MS, timer2_OISR); 
}

//Interrupcao pelo timer 1 por overflow
void timer1_OISR(){
  if (samples<numberSamples){
    if(enableAcquisition==true){
      //Lendo dado analogico
    sensorData[0]=sensorData[0]+analogRead(A0);
    sensorData[1]=sensorData[1]+analogRead(A1);
    sensorData[2]=sensorData[2]+analogRead(A2);
    sensorData[3]=sensorData[3]+analogRead(A3);
    sensorData[4]=sensorData[4]+analogRead(A4);
    sensorData[5]=sensorData[5]+analogRead(A5);
  samples++;
    }   
  }
}

//Interrupcao pelo timer 2 por overflow
void timer2_OISR(){
  if(enableAcquisition){
    digitalWrite(ACQ_LED_PIN, digitalRead(ACQ_LED_PIN) ^ 1);
  }else{
    digitalWrite(ACQ_LED_PIN, LOW);
  }
}

//FUNCAO QUE CONTROLA OS COMANDOS/SAIDAS-DIGITAIS
void commandOutput(){
  for(int i=0;i<sizeof(COMMAND_PORTS);i++){
    if(commandInstruction[i]){
      digitalWrite(COMMAND_PORTS[i],HIGH);
    }else{
      digitalWrite(COMMAND_PORTS[i],LOW);
    }
  }

  #ifdef    DEBUG
       Serial.print(F("Estado dos Comandos Digitais: "));
       for(int i=0;i<sizeof(commandInstruction);i++){
            Serial.print(commandInstruction[i]);
       }
       Serial.println();
  #endif

  
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
    sensorData[i]=RESET;
   }
}

//Funcao que escreve os resultados da aquisicao na porta serial
void printResults(int * result){    
    Serial.print(result[0]);
    Serial.print(",");
    Serial.print(result[1]);
    Serial.print(",");
    Serial.print(result[2]);
    Serial.print(",");
    Serial.print(result[3]);
    Serial.print(",");
    Serial.print(result[4]);
    Serial.print(",");
    Serial.print(result[5]);
    Serial.print(".");
}

//Funcao que printa um cabecalho para debug
void debugStarterMenu(){
Serial.println(F("--------------------------------------------------------------------------------------------------------------------------------------------------\n--------------------------------------------------------------------------------------------------------------------------------------------------\nPrograma para aquisicao de dados analogicos atraves de 6 entradas analogicas usando o chip ATMEGA328p integrado com controle de 4 saidas digitais.\n--------------------------------------------------------------------------------------------------------------------------------------------------\nFeito Por: Joao Guimaraes \nComandos:\n      -Inicar Aquisicao: '+'\n      -Terminar Aquisicao: ','\n      -Mater Reset(Reseta samplingRate, numberOfSamples e comandos digitais para valores padroes): '.'\n      -Set numberOfSamples:\n           -1:         '!'\n           -10:        '\"'\n           -100:       '#'\n           -500:       '$'\n           -1k:        '%'\n           -10k:       '&'\n     -Set samplePeriod:\n           -1Hz:       'A'\n           -10Hz:      'B'\n           -100Hz:     'C'\n           -1kHz:      'D'\n           -10kHz:     'E'\n           -100kHz:    'F'\n      -Saidas Digitais:\n           -0000:      '`'\n           -0001:      'a'\n           -0010:      'b'\n           -...        ...\n           -1110:      'n'\n           -1111:      'o'\n--------------------------------------------------------------------------------------------------------------------------------------------------\n--------------------------------------------------------------------------------------------------------------------------------------------------\n"));
}

//Funcao setup - inicializada com arduino
void setup(){
  
  Serial.begin(9600);
  pinMode(ACQ_LED_PIN,OUTPUT);
  for(int i=0;i<4;i++){
     pinMode(COMMAND_PORTS[i],OUTPUT);
     digitalWrite(COMMAND_PORTS[i],LOW);
  }
  digitalWrite(ACQ_LED_PIN,LOW);

  configureTimer1();
  configureTimer2();
  MsTimer2::start();
  
  #ifdef DEBUG
    debugStarterMenu();
    Serial.println(F("PROGRAMA INICIADO\n"));
    Serial.print(F("Led para indicacao da acquisicao na porta: "));
    Serial.println(ACQ_LED_PIN);
    Serial.print(F("Portas de comandos (saidas digitais): "));
    
    for(int i=0;i<4;i++){
      Serial.print(COMMAND_PORTS[i]);
      Serial.print(F(","));
    }
    Serial.println("\n");
  #endif 
  

}

//Funcao loop - executada continuamente
void loop(){
  //parar o timer quando o numero de amostras for o desejado
  if(samples>=numberSamples){
    Timer1.stop();
    
    static int resultAcq[6];
    for(int counter=0;counter<6;counter++){
      resultAcq[counter] = (int)roundf((sensorData[counter]/numberSamples));    
      sensorData[counter]=RESET;
    }
    samples=RESET;
    Timer1.restart();   //reinciando o timer 

    printResults(resultAcq);
    
  }

  //Verificado dado da porta serial
  if (Serial.available()){
    
    //Lendo o byte mais recente
    char byteRead = Serial.read();

    switch(byteRead) {
      
      //Recebeu comando iniciar aquisicao
      case START_ACQ:{
        if(enableAcquisition==false){
          //Habilita a aquisicao

          #ifdef  DEBUG
            Serial.print(F("\nInciando Acquisicao (SampleRate: "));
            Serial.print(1*1000000/sampleRate);
            Serial.print(F(" Hz  & NumberOfSamples: "));
            Serial.print(numberSamples);
            Serial.println(F(")"));
          #endif

          
          enableAcquisition=true;
          samples=RESET;
          //Serial.println(numberSamples);
          //Serial.println(sampleRate);
          Timer1.restart();       
        }            
      }
      break;
      case STOP_ACQ:{
          //Desabilita a aquisicao
          acquisitionStop();
          
          #ifdef  DEBUG
            Serial.println(F("Acquisicao Parada"));
          #endif
                         
      }
      break;

      case MASTER_RESET:{
        acquisitionStop();
        resetCommandOutput();
        sampleRate=RATES[3];
        numberSamples=SAMPLES[2];
        configureTimer1(); 

        #ifdef  DEBUG
           Serial.print(F("Master Reset\n   Todos os comandos digitais zerados\n   SampleRate de volta para o valor padrao("));
           Serial.print(1*1000000/sampleRate);
           Serial.print(F(" Hz)\n   NumberOfSamples de volta para o valor padrao("));
           Serial.print(numberSamples);
           Serial.println(F(")"));
        #endif
         
      }
      break;
      
      default:{
        if (byteRead > 95){       //Controle dos comandos
          if (byteRead < 112){
            for (int i = 3; i>-1; i--) {
              commandInstruction[i]=((byteRead >> i) & 1);
            }
            commandOutput();
          }
        }
      }
      if(enableAcquisition==false){
          if (byteRead > 64){     //Controle da Sampling Rate (default 1KHz)
            if (byteRead <71){
              sampleRate=RATES[(byteRead-65)];
              Timer1.setPeriod(sampleRate);
              Timer1.stop();
              
              #ifdef  DEBUG
                Serial.print(F("Timer de acquisicao definido com frequencia de "));
                Serial.print(1*1000000/sampleRate);
                Serial.print(F(" Hz)\n"));
              #endif
              
            }
          }

          if (byteRead > 32){     //Controle da Number of Samples
            if (byteRead <39){
              numberSamples=SAMPLES[(byteRead - 33)];

              
              #ifdef  DEBUG
                Serial.print(F("NumberOfSamples do buffer de entrada definida em "));
                Serial.print(numberSamples);
                Serial.print(F(" amostra(s)\n"));
              #endif
            }
          }
        }

      break; 
    }
  }
}
