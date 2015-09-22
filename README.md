## UAVXArm ##

The UAVXArm32F4 Board has been available since December 2012 from QuadroUFO and is now at Version 4.

### General Specifications ###

UAVXArm32 processor is an F4 and it spends most of its time twiddling its thumbs (doing nothing). Control loop and motor updates are at ~500Hz with 2KHz possible. 

Standalone mode:

  * STM32F405RG6 with FPU 168MHz (1024Kb Flash/32Kb RAM)
  * 64MBit External Flash (Black Box recorder)
  * MPU6000 Gyro/Accelerometer
  * HMC5983 Magnetometer
  * MS5611 Barometer
  * SPI communications for all sensors
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

  * PWM (490Hz)
  * SyncPWM (400Hz)
  * SyncPWM/8 OneShot 
  * I2C @ 400KHz
  * DCMotors (with optional 6 channel external adapter)

We have SimonK and BLHeli ESC codes with FastPWM being the sensible minimum for good performance.

All the usual airframe types are supported including conventional. Use of UAVXArm32 for camera gimbal only control is possible.

Attitude Estimation Schemes:

  * Madgwick (IMU)
  * Madgwick (AHRS)

Attitude Control:

  * Angle (default - also called attitude or stable)
  * Rate (commonly called gyro mode)
  * a mix of the above
 
![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/Uavx-nanoV4.png](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/Uavx-nanoV4.png)

##Videos and Builds##

Ken has a good collection at:


<a href='http://www.youtube.com/watch?feature=player_embedded&v=fUZlUUwVRLw' target='_blank'><img src='http://img.youtube.com/vi/fUZlUUwVRLw/0.jpg' width='425' height=344 /></a>
