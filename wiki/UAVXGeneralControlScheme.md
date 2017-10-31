# UAVX General Control Scheme #

These are short notes on the overall control scheme used by UAVX. The emphasis is on keeping it simple. Various incarnations of UAVX have been flying for around a decade. The original motivation was camera platforms from Wolfgang Mahringer's UAVP. 

## RC Input ##

RC stick data packets arrive at either 9mS intervals for SBus or 18mS for CPPM. A few older systems update every 22.5mS. Large delays here dominate the feel of multicopters. On the face of it there is no reason that the packet rate from the Tx could not be much higher.

## Gyros and Accelerometers ##

The gyros and accelerometers are sampled at 500Hz for V3 I2C sensor based boards and 1KHz for V4 SPI based boards. Of course the V4 boards can comfortably support sampling rates of 8MHz or more but we have not found this to be necessary. The gyro and accelerometer data is very very noisy as may be expected. 

The gyro data is  filtered typically to 100Hz using a simple second order digital low pass filter (DLPF).  Gyro data is also slew limited typically to 2000 deg/s/s.

The accelerometer data is also filtered using a second order filter typically at 15Hz.  The MPU6xxx sensor DLPFs are not used.

The attitude angle estimate is is obtained by integrating the gyro data and fusing it with the accelerometer data (Madgwick). The accelerometer weighting is depends on the confidence we have in the accelerometer. This take the form of a bell centred on  1G. The distributions standard deviation can be set as desired but is typically 0.4.

The gyro data is temperature compensated. Drift is very low after using a once off two temperature calibration procedure. As a consequence the attitude angle estimates are adequate even if the accelerometer confidence , and hence weighting, falls close to zero.

### Prop Wash and External Disturbances ###

There are no gyro notch filters or any other special actions to remove prop wash.

Prop wash occurs as blades cross motor arms causing “rocking” impulses, and when a multicopter flies into its own wash. The latter situation is akin to Vortex rotor stall (VRS) that full size heli pilots are acutely aware of.  Motor arms are most commonly formed from flat (cheap) CF plates. With larger traditional quads and 12” props hover is around 4500 RPM or 150 arm crossings per second.  For racers the motor RPM is much higher and with typically 3 blade props. At that point we enter the domain/debate of apparently "requiring" astronomical sampling rates currently at up to 32KHz. Some of us believe this is solid gold speaker wire territory (Chad Nowak https://www.youtube.com/watch?v=1ZE2m6Kf6L8&t=2618s).

There is a danger in attempting to respond to propwash as it can lead to regenerative feedback. 

The propwash and other aircraft generated noise, as we have known for over a decade in this domain, may be reduced by judicious use of compliant mounts for motors and flight controller as long at does not itself resonate. It also helps if you do not, wherever possible, deliberately fly into your own wash!

## Loop Time Synchronisation ##

The attitude controllers generally are updated at the gyro/acc sampling rates although it is possible to reduce this rate and probably still obtain the same performance. For now we keep it simple.

We do not use any form of static scheduling or other RTOS. Interrupts to the main loop are the main cause of jitter. This includes receivers with CPPM or parallel inputs. These are now less commonly used and all other inputs are polled. DMA transfers are the remaining disruption.  

We spin on the microsecond clock until the loop start time. When this is reached the motor setpoints are updated with the values computed on the previous loop incurring a delay of 1 or 2mS. The sensors are then sampled and the next motor setpoint computed.  Other housekeeping tasks are then computed. The worst case time for housekeeping is well within the loop time budget. With the interrupt caveats above this reduces jitter and leads to good control over loop time. No assumptions, however, are made for I and D terms where the actual dT is used. 

### Roll and Pitch ###

Roll and pitch “stick” control can be angle, rate or a mix of both (horizon). The controller is formulated as PI-PD.

The outer angle PI loop is relatively insensitive to tuning changes. The inner rate PD loop is a different situation. The D term may be used to achieve quite high P parameters but as a consequence will be susceptible to prop wash effects.  Currently we use modest/low P parameters and reserve D for damping external disturbances which means it is relatively modest as well.

To compute D the gyro data is smoothed using a 4 stage MA filter, differentiated and filtered again using a single pole LPF typically at half the gyro filter cutoff. We also using Pavel’s differentiator as an alternative.

As an observation very little consideration is currently being given to mass distribution. Batteries are almost always well away from the prop plane.

### Yaw ###

Yaw stick controls rate using a PD controller.  The heading estimate is obtained by fusing the magnetometer with gyro and acc (Madgwick AHRS) or directly using only the magnetometer.  The controller is P only and feeds the rate loop.  Tuning is usually quite “gentle” as the magnetometer updates are at 75Hz. As an aside for fixed wing aircraft, or multicopters travelling faster than 1M/S we use the GPS heading made good if available. We do not currently attempt to fuse the magnetometer and GPS derived headings.

## Motors ##

Motor outputs are filtered at the same frequency as the gyro filters to prevent higher frequency signals being dissipated as heat. Motor rise times are typically now down to less than 50mS  compared to around 100mS a decade back (http://www.miniquadtestbench.com/). This excludes lag.

Motor setpoints are not permitted to fall below the idle setting; rescaling is used on the roll, pitch and yaw.

By default we use asynchronous PWM at 490Hz although a number of other protocols are available including brushed motors. Digital transmission would far more preferable to avoid the madness of pulse width measurement by the ESCs. We have supported I2C forever but it is no longer commonly available. It is unfortunate that DShot is upon us along with "faster is better" gyro sampling rates rather than something sane like broadcast SPI or CAN using the dedicated hardware in the Arms ;).

## Latency/Lag ##

Latency is approximately 5mS for each filter pole leading to a current loop latency of 4x5 or 20mS for gyro filter at 100Hz. The rate D term has an additional (5+2)mS delay. The accelerometer filter delay at 20Hz is approximately 21mS.

This does not include RC lag.

G.K. Egan Oct 2017
