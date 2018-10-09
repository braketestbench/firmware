void setup(){
	Serial.begin(115200);
	Serial.print("Test application for Serial Port\n");
}

void loop(){
	if (Serial.available()){
		unsigned char byteRead = Serial.read();

		switch (byteRead){
			case 97:
				Serial.print("String a\n");
			break;

			case 98:
				Serial.print("String b\n");
			break;
		}
	}
}