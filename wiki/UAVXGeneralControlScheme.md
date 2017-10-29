# UAVX General Control Scheme #

These are short notes on the overall control schemes used by UAVX. The emphasis is on keeping it simple. Various incarnations of UAVX have been flying for around a decade.

## RC Input ##

RC stick data packets arrive at either 9mS intervals for SBus or 18mS for CPPM. A few older systems update at  every 22.5mS.

## Gyros and Accelerometers ##

The gyros and accelerometers are sampled at 500Hz for V3 I2C sensor based boards and 1KHz for V4 SPI based boards. Of course the V4 boards can comfortably support loop times of 8MHz or more but we have not found this to be necessary. The gyro and accelerometer data is very very noisy as may be expected. The noise may be reduced by judicious use of compliant mounts for motors and flight controller as long at does not itself resonate.

The gyro data is  filtered typically to 100Hz using a simple second order digital low pass filter.  Gyro data is also slew limited typically to 2000 deg/s/s.

The accelerometer data is also filtered using a second order filter typically at 15Hz.  The MPU6xxx sensor DLPFs are not used.

The attitude angle estimate is is obtained by integrating the gyro data and fusing it with the accelerometer data (Madgwick IMU). The accelerometer weighting is reduced using a bell shaped decay as the acceleration magnitude moves away from 1G.

The gyro data is temperature compensated. Drift is very low after using a once off two temperature calibration procedure. As a consequence the attitude angle estimates are adequate even if the accelerometer weighting falls to zero.

### Prop Wash and External Disturbances ###

There are no gyro notch filters or any other special actions to remove prop wash.

Prop wash mainly occurs as blades cross motor arms causing “rocking” impulses.  With larger quads and 12” props will be around 4500 RPM  at hover or 150 arm crossings per second.  For racers the motor RPM is much higher and with typically 3 blade props. At that point we enter the domain/debate of apparently requiring astronomical sampling rates currently at 32KHz.

## Control Modes ##

The attitude controllers generally are updated at the gyro/acc sampling rates.

### Roll and Pitch ###

Roll and pitch “stick” control can be angle, rate or a mix of both (horizon). The controller is formulated as PI-PD.

The outer angle PI loop is relatively insensitive to tuning changes. The inner rate PD loop is a different situation. The D term may be used to achieve quite high P parameters but as a consequence will be susceptible to prop wash effects.  Currently we use low P parameters and reserve D for damping external disturbances which means it is relatively low as well.

To compute D the gyro data is smoothed using a 4 stage MA filter, differentiated and filtered again using a single pole LPF typically at half the gyro filter cutoff. We also using Pavel’s differentiator as an alternative.

### Yaw ###

Yaw stick controls rate using a PD controller.  The heading estimate is obtained by fusing the magnetometer with gyro and acc (Madgwick AHRS) or directly using only the magnetometer.  The controller is P only and feeds the rate loop.  Tuning is usually quite “gentle” as the magnetometer updates are at 75Hz. For Fixed wing aircraft, or multicopters travelling faster than 1M/S we use the GPS heading made good if available. We do not currently attempt to fuse the magnetometer and GPS derived headings.

## Motors ##

Motor outputs are filtered at the same frequency as the gyro filters to prevent higher frequency signals being dissipated as heat. Motor rise times are typically now down to less than 50mS  compared to around 100mS a decade back. This excludes lag.

By default we use asynchronous PWM at 490Hz although a number of other protocols are available. Digital transmission would far more preferable to avoid the madness of pulse widthe measurement by the ESCs. We have supported I2C forever but it is no longer commonly supported. It is unfortunate that DShot is upon us along with faster is better gyro sampling rates ;).

## Latency ##

Latency is approximately 3mS for each filter pole leading to a current loop latency of 4x3 or 12mS. The rate D term has an additional 3mS delay. 

The RC lag is an additional 9 to 18mS plus a single pole filter.

G.K. Egan Oct 2017
