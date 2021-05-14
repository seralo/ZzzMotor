#include <ZzzStepper.h>
#include <Wire.h>

ZzzMotorDriverDirectionPWM<26, 36> motorDriver();
ZzzStepper stepper(motorDriver);

int nbSeconds=0;

void motorStopped() {
	nbSeconds++;
	Serial.print("Seconds:");
	Serial.println(nbSeconds);

	if (nbSeconds>=10) {
		Serial.println("Stop");
		//motor.stop(); //is not be needed goMs() call stop after given ms();
		return;
 	}
	motor.goMs(1000, true, motorStopped);
}

void setup() {
	Serial.begin(115200);
	delay(250); //to ensure correctly initialized

	Serial.println("Motor demo");

	//set rotation speed to 50%
	motor.setSpeed(50);

	//ask to run clockwise for given milliseconds then call function motorStopped() at the end 
	motor.goMs(1000, true, motorStopped);
}

void loop() {
	motor.update();
}
