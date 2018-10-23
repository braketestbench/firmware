//Analog Inputs
#define CKP_SIG_1                                       A0
#define THERM_SIG_1                                     A1
#define THERM_SIG_2                                     A2
#define LOAD_SIG_1                                      A3
#define LOAD_SIG_2                                      A4
#define VIB_SIG_1                                       A5
#define AOUT_SIG_1                                      A7
#define AOUT_SIG_2                                      A8

int read =0;		

void setup(){
	Serial.begin(115200);
	Serial.print("====Test application for Analog Input Ports====\n");
}

void loop(){
	if (Serial.available()){
	       unsigned char byteRead = Serial.read();

	       if (byteRead==97){
                        
                        read++;
                        float[8] reads;
                        reads[0]=analogRead(CKP_SIG_1);
                        reads[1]=analogRead(THERM_SIG_1);
                        reads[2]=analogRead(THERM_SIG_2);
                        reads[3]=analogRead(LOAD_SIG_1);
                        reads[4]=analogRead(LOAD_SIG_2);
                        reads[5]=analogRead(VIB_SIG_1);
                        reads[6]=analogRead(AOUT_SIG_1);
                        reads[7]=analogRead(AOUT_SIG_2);

                        for(int i=0;i<reads.length();i++){
                                reads[i]=reads[i]*5/1023;
                        }

                        Serial.print("\nANALOG READ N%d\n----\n",read);

                        Serial.print("CKP_SIG_1 Read: %0.0f\n",reads[0]*1023/5);
                        Serial.print("CKP_SIG_1 Voltage: %0.2f V\n",reads[0]);
                        Serial.print("CKP_SIG_1 Freq: %0.2f Hz \n",(reads[0]-1.224)/123);
                        Serial.print("----\n");

                        Serial.print("THERM_SIG_1 Read: %0.0f\n",reads[1]*1023/5);
                        Serial.print("THERM_SIG_1 Voltage: %0.2f V\n",reads[1]);
                        Serial.print("THERM_SIG_1 Temp %0.2f C \n",(reads[1]*200));
                        Serial.print("----\n");

                        Serial.print("THERM_SIG_2 Read: %0.0f\n",reads[2]*1023/5);
                        Serial.print("THERM_SIG_2 Voltage: %0.2f V\n",reads[2]);
                        Serial.print("THERM_SIG_2 Temp %0.2f C \n",(reads[2]*200));
                        Serial.print("----\n");

                        Serial.print("LOAD_SIG_1 Read: %0.0f\n",reads[3]*1023/5);
                        Serial.print("LOAD_SIG_1 Bridge Voltage: %0.2f mV\n",reads[3]*1000/996);
                        Serial.print("LOAD_SIG_1 Voltage: %0.2f V\n",reads[3]);
                        Serial.print("LOAD_SIG_1 Force %0.2f KN \n",(reads[3]*100.4));
                        Serial.print("----\n");

                        Serial.print("LOAD_SIG_2 Read: %0.0f\n",reads[4]*1023/5);
                        Serial.print("LOAD_SIG_2 Bridge Voltage: %0.2f mV\n",reads[4]*1000/996);
                        Serial.print("LOAD_SIG_2 Voltage: %0.2f V\n",reads[4]);
                        Serial.print("LOAD_SIG_2 Force %0.2f KN \n",(reads[4]*100.4));
                        Serial.print("----\n");

                        Serial.print("VIB_SIG_1 Read: %0.0f\n",reads[5]*1023/5);
                        Serial.print("VIB_SIG_1 Voltage: %0.2f V\n",reads[5]);
                        Serial.print("VIB_SIG_1 Vib %0.2f g \n",((reads[5]*1.82)-3));
                        Serial.print("----\n");
                }   
	}
}