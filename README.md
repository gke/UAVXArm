## UAVXArm ##

The UAVXArm32F4 Board has been available since December 2012 from QuadroUFO and is now at V4.

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/v4.jpg](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/v4.jpg)

The V4 is probably the smallest full-house F4 board out there and has been flown on regular 600 scale octocopters down to nano quads. In the latter case a daughter board also available from QuadroUFO provides1/2S support for up to 6 brushed motors.

While designed for UAVX the V4 board is a simple porting target for other flight codes which have now adopted, or support, the same sensor package.

### UAVX Functionality ###

UAVX continues to support the functions and failsafe features it has had for almost a decade including return to home and waypoint navigation. Recent additions include:

  * VRS avoidance
  * Inflight vibration monitoring using using DFTs.
  * Bad motor/ESC diagnostics.
  * SyncPWM and oneShot although some/many remain unconvinced.
  * FrSky and MAVLink telemetry intended principally for minimOSD.
  * Representative physics for emulation mode.
  * Single UAVXGUI groundstation application.
  * WS2812 LED support for pretty lights.

### General Specifications ###

UAVXArm32F4 spends most of its time twiddling its thumbs (doing nothing) as it has so much computational capacity. Control loop and motor updates are at ~500Hz with 2KHz possible.

  * STM32F405RG6 with FPU 168MHz (1024Kb Flash/32Kb RAM)
  * 64MBit External Flash (Black Box recorder)
  * MPU6000 Gyro/Accelerometer
  * HMC5983 Magnetometer
  * MS5611 Barometer
  * SPI communications for primary sensors
  * Ultrasonic Rangefinder
  * Optional external battery voltage and current monitor
  * Analog and I2C ports for alternative sensors

Receivers:

  * Parallel PPM (8 channels)
  * Compound PPM
  * Spektrum Satellite (1024b and 2048b resolution)
  * Deltang
  * SBus (with external inverter)

A minimum of 4 channels is required for basic flight control with 7 channels being required for RTH and Nav functionality.

Up to 10 drives or servos:

  * FastPWM (490Hz)
  * SyncPWM (400Hz)
  * SyncPWM/8 OneShot 
  * I2C @ 400KHz
  * DC Motors (with optional 6 channel external adapter)

We have used SimonK and BLHeli ESC codes with FastPWM being the sensible minimum for good performance.

All the usual airframe types are supported including conventional fixed wing airframes. 

Attitude Estimation Schemes:

  * Madgwick (IMU)
  * Madgwick (AHRS)

Attitude Control:

  * Angle (default - also called attitude or stable)
  * Rate (commonly called gyro mode)
  * a mix of the above

###Videos and Builds###

There are many many many videos of UAVX on our RCG thread and elsewhere:

http://www.rcgroups.com/forums/showthread.php?t=1093510

Ken the main Alpha tester has a collection at https://www.youtube.com/user/kb6mcc.

https://www.youtube.com/watch?v=faXzluGCzVo&feature=youtu.be (Indoors DC Motors)






