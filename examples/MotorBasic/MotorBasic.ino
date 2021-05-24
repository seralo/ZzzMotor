#include <ZzzMotor.h>
#include <Wire.h>

ZzzMotorDriver2PinsPWM<32, 33> motorDriver;
//Without PWM only 100% speed ZzzMotorDriver2Pins<32, 33> motorDriver;
ZzzMotor motor(motorDriver);

int nbSteps=0;
int duration=1000; //1 seconds
int speed=100;
bool cw=true;

void motorEvent() {
	nbSteps++;
	Serial.print("Steps:");
	Serial.println(nbSteps);

	if (nbSteps>=20) {
		Serial.println("Stop");
		//motor.stop(); //is not needed goMs() call stop after given ms();
		return;
 	}
	switch(nbSteps%5) {
		case 0:
			//stop (speed 0) for duration and change direction
			motor.setSpeed(0);
			motor.goMs(duration, cw, motorEvent);
			cw=!cw;
			speed=100;
			break;
		default:
			Serial.print("Clockwise:");
			Serial.print(cw);
			Serial.print(" Speed:");
			Serial.println(speed);
			motor.setSpeed(speed);
			speed-=10;
			motor.goMs(duration, cw, motorEvent);
			break;
	}
}

void setup() {
	Serial.begin(115200);
	delay(250); //to ensure correctly initialized

	Serial.println("Motor demo");

	//set rotation speed
	motor.setSpeed(speed);

	//ask to run clockwise for given milliseconds then call function motorStopped() at the end 
	motor.goMs(duration, cw, motorEvent);
}

void loop() {
	motor.update();
}
