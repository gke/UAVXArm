## UAVXArm ##

The UAVXArm32F4 Board has been available since December 2012 from QuadroUFO:

> http://www.quadroufo.com/index.php?cPath=28&osCsid=p7clqph0gaebcfgjok17p59k15

Startup Wiki:

> http://code.google.com/p/uavp-mods/wiki/UAVXArm32F4Startup.

### August 2012 ###

Videos first and specifications etc below.

### UAVP Adapter Mode ###

<a href='http://www.youtube.com/watch?feature=player_embedded&v=gFUiPCce--4' target='_blank'><img src='http://img.youtube.com/vi/gFUiPCce--4/0.jpg' width='425' height=344 /></a>

### Standalone Mode ###

<a href='http://www.youtube.com/watch?feature=player_embedded&v=fUZlUUwVRLw' target='_blank'><img src='http://img.youtube.com/vi/fUZlUUwVRLw/0.jpg' width='425' height=344 /></a>

### General Specifications ###

UAVXArm32 processor is an F4 and it spends most of its time twiddling its thumbs (doing nothing). Control loop and motor updates are at ~500Hz with 2KHz possible. The control computations are around 52us (millionths of a second).

![http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArmF4.jpg](http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVXArmF4.jpg)

The MPU6050 DMP engine is not used currently and may not be because of update rate limitations and documentation (200Hz and lack of..).

UAVXArm32 can be used standalone or as an adapter on the "Legacy" UAVP board making use of its sensors including the analog gyros which are still hard to beat.

Standalone mode:

  * STM32F405RG6 with FPU 168MHz (1024Kb Flash/32Kb RAM)
  * 512KBit EEPROM
  * MPU6050 Gyro/Accelerometer
  * HMC5883L Magnetometer
  * MS5611 Barometer
  * Ultrasonic Rangefinder
  * Optional external battery voltage and current monitor
  * Analog and I2C ports for alternative sensors

I2C sensor communications are at 400KHz (sensor limitations).

Additional UAVP Sensors supported in adapter mode:

  * All analog gyros
  * HMC6552 Compass (I2C limited to 100KHz)
  * BMP085 Barometer

Receivers:

  * Parallel PPM (8 channels)
  * Compound PPM (FrSky etc or external decoder)
  * Spektrum Satellite (1024b and 2048b resolution)
  * maybe SBus (with external inverter)

A minimum of 4 channels is required for basic flight control with 7 channels being required for RTH and Nav functionality.

Drives:

  * Fast PWM @ 450Hz(6 channels or 10 without Parallel PPM input)
  * I2C at 400KHz

We are using SimonK ESC codes with FastPWM being the sensible minimum for good performance.

All the usual airframe types are supported including conventional. Use of UAVXArm32 for camera gimbal only control is possible.

Attitude Estimation Schemes:

  * Wolferl (original UAVP)
  * Madgwick (IMU)
  * Madgwick (AHRS)
  * DMP - eventually

Attitude Control:

  * Rate (default)
  * Angle (commonly called stable mode)

Camera stabilisation:

  * Two servo channels tracking pitch and roll

Navigation and Failsafes:

  * as for UAVXPIC

Navigation performance is determined by GPS and GPS system intrinsic limitations. 5Hz update rate is a minimum. $GPGGA and $GPRMC sentences are used with other sentences being ignored. UBlox binary input possible if required. UBlox GPS units are popular and appear to give good performance when its various filters are set to pedestrian.

Tools:

  * UAVPSet
  * UAVXGS
  * UAVXNav (in progress)
  * STM Flash Programmer

MavLink and MultiWii all possible now we have code space;).

Programming uses serial telemetry port with no special programmer being required.


## Past Reports ##

#### April 2012 ####

We initially decided to do a UAVXArmLite board based on the very popular STM32F103 Arm chip. The board is designed at Ken's suggestion, to be used either standalone, or as the final-final-final UAVP update plugging into the PIC socket to make use of the various existing sensors on the UAVP board and in particular the analog gyros and accelerometer. The board has a socket into which the baro and a magnetometer assembly may be plugged as the Lite board covers this area. In standalone mode this socket may be used to attach other sensor/imu boards as they become available including the MPU6150. The STM32F407 may follow later in the year but don't hold your breath.

Ken currently intends to make one board without sensors to plug into the UAVP and a second version with all of the sensors these being the MPU6050, HMC5883L magnetometer and the MS5611 Baro. The board with the full range of sensors can still mount on the UAVP board if you want to use the analog gyros.