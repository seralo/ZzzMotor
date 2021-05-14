/***************************************************************
  ZzzMotor.h
***************************************************************/
#ifndef ZZZ_MOTOR_H
#define ZZZ_MOTOR_H

typedef void(*ZzzMotorCallback)();

/** Abstract motor driver. Class to override to implement a new motor driver */
class ZzzMotorDriver {
	protected:
		ZzzMotorDriver() {
		}
	public:
		/** Send stop command to motor */
		virtual bool stop();

		/** Ask motor to start running at given speed */
		virtual bool go(bool cw, int speed);

		/**
		 * Check speed parameter and return a supported speed which can be given to go().
		 * @speed is a percentage of motor speed. 0 is stopped, 100 is full speed.
		 * @return -1 if invalid parameter given. or the actual speed set.
		 */
		virtual int checkSpeed(int speed) {
			if (speed<0 || speed>100) {
				return -1;
			}
			return speed;
		}
};


/**
 * PIN_DIRECTION specify the pin to control the motor direction
 * PIN_PWM specify the pin to control the speed
 */
template <int PIN_DIRECTION, int PIN_PWM> class ZzzMotorDriverDirectionPWM : public ZzzMotorDriver {
	public:
		ZzzMotorDriverDirectionPWM() {
			pinMode(PIN_DIRECTION, OUTPUT);
			pinMode(PIN_PWM, OUTPUT);
		}

		virtual bool stop() override {
			digitalWrite(PIN_PWM, 0);
			return true;
		}

		virtual bool go(bool cw, int speed) override {
			digitalWrite(PIN_DIRECTION, cw ? HIGH : LOW);
			if (speed==100) {
				digitalWrite(PIN_PWM, HIGH : LOW);
			}
			else {
				int pwm=map(speed, 0, 100, 0, 255);
				analogWrite(PIN_PWM, pwm);
			}
			return true;
		}
};

/* FIXME to implement: continous servo */


/* FIXME to implement
template <typename WIRE, uint8_t MOTOR_PORT, uint8_t ADDRESS=ZZZ_DEFAULT_M5STACKPBHUB_ADDRESS> class ZzzMotorDriverM5StackPBHub : public ZzzMotorDriver {
	protected:
		WIRE *_pWire;
	public:
		ZzzMotorDriverM5StackPBHub(void* pParams) {
			_pWire=(WIRE*)pParams;
			_pWire->begin();

			_pWire->beginTransmission(ADDRESS);
  			_pWire->write(0);
 			if (_pWire->endTransmission() != 0) { //communication error
 				return;
			}
		}
		
		virtual bool stop() override {
			if (force) {
				_pWire->beginTransmission(ADDRESS);
	  			_pWire->write(0);
	 			if (_pWire->endTransmission() != 0) { //communication error
	 				return false;
				}
			}
			return true;
		}

		virtual bool go(bool cw, int speed) override {
			uint8_t stepState;

			stepState=_steps.nextStep( cw );
			_pWire->beginTransmission(ADDRESS);
	  		_pWire->write(stepState);
	 		if (_pWire->endTransmission() != 0) { //communication error
	 			return false;
			}
			return true;
		}
}; */



/**
 * Class to manage a motor. The class need a Driver parameter to control the motor.
 * The driver class must implement stop(bool force), go(bool cw, speed) and checkSpeed(speed).
 *
 * TODO do driver call in update ? to limit too rapid motor change
 * NOTE (going full speed clockwise, then full speed the other way may break the motor, could add timing limit defined in the driver)
 */
class ZzzMotor {
	protected:
		/** Stop state: all flags set */
		static const int STATE_STOP=-1;
		/** Clockwise */
		static const int STATE_CW=1;
		/** Counter-clockwise */
		static const int STATE_CCW=0;
		
		static const int STATE_GO_TIME=8;

		/** Bit state (CW, CCW) Steps, continuous */
		int _state;

		/** Value between 0 - 100 */
		int _speed;

		/** Timer for goMs (2 values to make it overflow proof) */
		unsigned long _timerStartMs;
		unsigned long _timerDurationMs;
		
		ZzzMotorDriver *_pDriver;

		/** Callback called at the end of goMs  */
		ZzzMotorCallback _endActionCallback;

		void endAction() {
			_pDriver->stop();
			_state=STATE_STOP;
			if (_endActionCallback!=nullptr) {
				_endActionCallback();
			}
		}

	public:		
		/** Stop the stepper motor */
		void stop() {
			_pDriver->stop();
			_state=STATE_STOP;
		}

		/** Is the stepper motor running (not stopped) */
		bool isRunning() {
			return (_state!=STATE_STOP);
		}

		/** To call frequently (ie: in Arduino loop) for goMs() */
		void update() {
			if (_state==STATE_STOP) {
				return; //nothing to do
			}
			if ((_state & STATE_GO_TIME)==STATE_GO_TIME) {
				if (millis() - _timerStartMs >_timerDurationMs) {
					endAction();
					return;
				}
			}
		}
		
		/** Turn until stop (async function need to call update() frequently) */
		void go(bool cw=true) {
			_state=(cw ? STATE_CW : STATE_CCW);
			_pDriver->go(cw, _speed);
		}

		/** Number of milliseconds (time) to run (async function need to call update() frequently) */
		void goMs(unsigned long ms, bool cw=true, ZzzMotorCallback endActionCallback=nullptr) {
			go(cw);
			_timerDurationMs=ms;
			_timerStartMs=millis();
			_state=(cw ? STATE_CW : STATE_CCW) | STATE_GO_TIME;
			_endActionCallback=endActionCallback;
		}


		/** Set motor speed. If the motor is running speed will be adjusted. */
		int setSpeed(int speed) {
			int validSpeed=_pDriver->setSpeed(speed);
			if (validSpeed!=-1) {
				_speed=validSpeed;
				if (_state!=STATE_STOP) {
					go(((_state & STATE_CW) == 1), _speed);
				}
			}
			return validSpeed;
		}

		/** Constructor */
		ZzzMotor(ZzzMotorDriver &driver) {
			_pDriver=&driver;
			_state=STATE_STOP;
			_speed=setSpeed(100);
		}
};

#endif

