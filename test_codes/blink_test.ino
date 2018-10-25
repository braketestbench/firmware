void setup(){
	pinMode(15,OUTPUT);
	digitalWrite(15,LOW);
}

void loop(){
	digitalWrite(15,(digitalRead(15)^1));
	dealy(30);
}