# UAVX Telemetry Only for OpenTx #

This version generates V2 FrSky telemetry for transmitters with OpenTx V2.1 onwards. A Smart Port version may follow.

## Frsky Protocol ##

FrSky "D" type receivers transmit RSSI and up to two voltages with no external sensors. This version of UAVX generates telemetry packets in a similar way to the sensor hub (now superseded) did allowing the connection of a UAVXArm board as a multi sensor device. 

The full packet protocol is described elsewhere but suffice it to say each sensor has a unique ID which appears on the Tx. Many of the allowable sensor IDs are already defined for FrSky sensors and will be displayed on the Tx when using "discover new sensors". 
	
### UAVX Setup ###

Load the telemetry version of the firmware using the normal procedure. UAVX is then configured using UAVXGUI with only the telemetry port connected. You will need to calibrate the IMU and Magnetometer as usual. Configuration is as normal selecting UAVX telemetry (NOT FrSky) and arming by switch. Note: The arming switch needs to set to disarmed otherwise the board will switch to FrSky telemetry at 9600baud and not communicate with the GUI.

### Hardware Setup ###

Connect the FrSky receiver to the old RC2.

### Operation ###

Consult the OpenTx documentation to discover what sensor values are being received by the transmitter.