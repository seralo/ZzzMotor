# ZzzMotor
Simple Arduino / C++ library to control motor


The library consist of a single header file (ZzzMotor.h) containing template classes.


### Constructor

```cpp

ZzzMotor motor(driver); //Constructor need a driver class as template param


```

### Functions

```cpp
void go(clockwise=true);  // Turn until stop (async function need to call update() frequently)
void update();            // To call in Arduino loop
void stop();              // Stop the motor

void goMs(ms, clockwise=true, endActionCallback=null); // Turn motor for given milliseconds

int setSpeed(percent);    // Set rotation speed in percent 0-100%. Return the speed set or -1 in case of error

bool isRunning();         // Is the motor running (not stopped)
```

### Included examples

- `MotorBasic/MotorBasic.ino` - Show basic usage changing speed and direction.


### Simple code example 

```cpp
#include <ZzzMotor.h>

//Basic driver on 2 pins connected here to PIN 32 (forward pin), PIN 33 (backward pin)
ZzzMotorDriver2PinsPWM<32, 33> motorDriver;
ZzzMotor motor(motorDriver);

void setup()
{
    ...
    motor.go();
}

void loop()
{
    ...

    motor.update();

    ...
}
```

