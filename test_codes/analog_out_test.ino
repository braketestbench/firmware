#include <math.h>

//Analog Inputs
#define AOUT_SIG_1					A7
#define AOUT_SIG_2					A8

//Analog Outputs
#define	PWM_OUT_1					9
#define	PWM_OUT_2					10			

void setup(){
	Serial.begin(115200);
	Serial.print("====Test application for Analog Ports====\n");
}

void loop(){
	if (Serial.available()){
	       unsigned char byteRead = Serial.read();

	       if ((byteRead >= CH1_NULL_SPEED_BYTE) &&  (byteRead <= CH1_MAX_SPEED_BYTE)){
			int dtOut= (255*(byteRead - CH1_NULL_SPEED_BYTE)/(CH1_MAX_SPEED_BYTE - CH1_NULL_SPEED_BYTE));
			float vout=dtout*5/255;
			float dtc=dtOut*100/255;
                        analogOut(PWM_OUT_1,dtOut);
        	        float vin = analogRead(AOUT_SIG_1)*5/1023;
        	        Serial.print("==Analog Output Channel 1==\n");
                	Serial.print("DutyCyle: %0.2f\n",dtc);
                	Serial.print("Aout Calculated: %0.2f V\n",vout);
                	Serial.print("Aout Read: %0.2f V\n",vin);
                	Serial.print("Error: %0.2f %\n\n",(abs(100*(vout-vin)/vout)));
                }

                if ((byteRead >= CH2_NULL_SPEED_BYTE) &&  (byteRead <= CH2_MAX_SPEED_BYTE)){
        	        int dtOut= (255*(byteRead - CH2_NULL_SPEED_BYTE)/(CH2_MAX_SPEED_BYTE - CH2_NULL_SPEED_BYTE));
			float vout=dtout*5/255;
			float dtc=dtOut*100/255;
        	        analogOut(PWM_OUT_2,dtOut);
        	        float vin = analogRead(AOUT_SIG_2)*5/1023;
        	        Serial.print("==Analog Output Channel 2==\n");
                        Serial.print("DutyCyle: %0.2f\n",dtc);
                	Serial.print("Aout Calculated: %0.2f V\n",vout);
                	Serial.print("Aout Read: %0.2f V\n",vin);
                	Serial.print("Error: %0.2f %\n\n",(abs(100*(vout-vin)/vout)));
                }
	}
}