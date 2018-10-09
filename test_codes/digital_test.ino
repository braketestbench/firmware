#define DIG_OUT_1		        	3
#define	DIG_OUT_2		        	2
#define	DIG_OUT_3					0

void setup(){
	Serial.begin(115200);
	Serial.print("Test application for Digital Ports\n");
	pinMode(DIG_OUT_1,OUTPUT);
	pinMode(DIG_OUT_2,OUTPUT);
	pinMode(DIG_OUT_3,OUTPUT);
	digitalWrite(DIG_OUT_1,HIGH);
	digitalWrite(DIG_OUT_2,HIGH);
	digitalWrite(DIG_OUT_3,HIGH);
}

void loop(){
	if (Serial.available()){
		unsigned char byteRead = Serial.read();

		switch (byteRead){
			case 49:
				digitalWrite(DIG_OUT_1,LOW);
				Serial.print("Output 1 = ON\n");
			break;

			case 50:
				digitalWrite(DIG_OUT_1,HIGH);
				Serial.print("Output 1 = OFF\n");
			break;

			case 51:
				digitalWrite(DIG_OUT_2,LOW);
				Serial.print("Output 2 = ON\n");
			break;

			case 52:
				digitalWrite(DIG_OUT_2,HIGH);
				Serial.print("Output 2 = OFF\n");
			break;

			case 53:
				digitalWrite(DIG_OUT_3,LOW);
				Serial.print("Output 3 = ON\n");
			break;

			case 54:
				digitalWrite(DIG_OUT_3,HIGH);
				Serial.print("Output 3 = OFF\n");
			break;

			default:
				digitalWrite(DIG_OUT_1,HIGH);
				digitalWrite(DIG_OUT_2,HIGH);
				digitalWrite(DIG_OUT_3,HIGH);
				Serial.print("All Outputs OFF\n");
			break;
		}
	}
}