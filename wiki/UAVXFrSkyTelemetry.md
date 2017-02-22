# UAVX Telemetry Only for OpenTx #

This version generates V2 FrSky telemetry for transmitters with OpenTx V2.1 onwards. A Smart Port version may follow.

## Frsky Protocol ##

FrSky "D" type receivers transmit RSSI and up to two voltages with no external sensors. This version of UAVX generates telemetry packets in a similar way to the sensor hub (now superseded) did allowing the connection of a UAVXArm board as a multi sensor device. 

The full packet protocol is described elsewhere but suffice it to say each sensor has a unique ID which appears on the Tx. Many of the allowable sensor IDs are already defined for FrSky sensors and will be displayed on the Tx when using "discover new sensors". 
UAVX has several other sensors and values which use unassigned IDs. The sensor menu on the Tx may be used to assign names to these new IDs. Not all of the assigned IDs are active because of a FrSky limit on the number of active sensors (or packet traffic) through the Rx. 

	FS_ID_GPS_ALT_BP = 0x01,
	FS_ID_TEMP1 = 0x02, // Barometer temperature deg C
	FS_ID_RPM = 0x03,
	FS_ID_FUEL = 0x04, // % battery used (nominal capacity set using GUI)
	FS_ID_TEMP2 = 0x05,
	FS_ID_VOLTS = 0x06,

	// 0x07
	// 0x08

	FS_ID_GPS_ALT_AP = 0x09,
	
	// 0x0A
	// 0x0B
	// 0x0C
	// 0x0D
	// 0x0E
	// 0x0F // seems to be emitted when there is a buffer overrun in the Rx.
	
	FS_ID_BARO_ALT_BP = 0x10,

	FS_ID_GPS_SPEED_BP = 0x11,
	FS_ID_GPS_LONG_BP = 0x12,
	FS_ID_GPS_LAT_BP = 0x13,
	FS_ID_GPS_COURS_BP = 0x14,

	FS_ID_GPS_DAY_MONTH = 0x15,
	FS_ID_GPS_YEAR = 0x16,
	FS_ID_GPS_HOUR_MIN = 0x17,
	FS_ID_GPS_SEC = 0x18,

	FS_ID_GPS_SPEED_AP = 0x19, // +8 from BP
	FS_ID_GPS_LONG_AP = 0x1A,
	FS_ID_GPS_LAT_AP = 0x1B,
	FS_ID_GPS_COURS_AP = 0x1C,

	UAVX_ID_GPS_STAT = 0x1d,
	// 0x1e
	// 0x1f
	// 0x20

	FS_ID_BARO_ALT_AP = 0x21,

	FS_ID_GPS_LONG_EW = 0x22,
	FS_ID_GPS_LAT_NS = 0x23,

	FS_ID_ACCEL_X = 0x24, // m/s2
	FS_ID_ACCEL_Y = 0x25,
	FS_ID_ACCEL_Z = 0x26,

	FS_ID_CURRENT = 0x28,

	UAVX_ID_WHERE_DIST = 0x29, // metres the aircraft is way
	UAVX_ID_WHERE_BEAR = 0x2a, // bearing (deg) to aircraft
	UAVX_ID_WHERE_ELEV = 0x2b, // elevation (deg) of the aircraft above the horizon
	FS_ID_WHERE_HINT = 0x2c, // which to turn to come home intended for voice guidance

	UAVX_ID_COMPASS = 0x2d, // deg
	// 0x2e
	// 0x2f

	FS_ID_VARIO = 0x30,

	//--------------------

	// UAVX user defined

	UAVX_ID_GYRO_X = 0x31, // deg/sec
	UAVX_ID_GYRO_Y = 0x32,
	UAVX_ID_GYRO_Z = 0x33,

	UAVX_ID_PITCH = 0x34, // deg
	UAVX_ID_ROLL = 0x35,

	UAVX_ID_MAH = 0x36, // mAH battery consumption

	//--------------------

	FS_ID_VFAS = 0x39,
	FS_ID_VOLTS_BP = 0x3A,
	FS_ID_VOLTS_AP = 0x3B,
	FS_ID_FRSKY_LAST = 0x3F,
	
Rather than do a blow by blow of what is (@December 2016) sent and when, here is the telemetry routine:

	void SendFrSkyTelemetry(uint8 s) {
	static uint8 FrameCount = 0;

	if (++FrameCount == 40) { // FRAME 3 every 8 seconds
		TxFrSkyTime(s);
		TxFrSkyTemperature1(s);
		TxChar(s, FS_SENTINEL);

		FrameCount = 0;

	} else if ((FrameCount % 5) == 0) { // FRAME 2 every second
		if (F.GPSValid) {
			if (F.OriginValid)
				TxFrSkyWhere(s);
			TxFrSkyGPSSpeed(s);
			TxFrSkyGPSAlt(s);
			TxFrSkyGPSHeading(s);
			TxFrSkyGPSCoords(s);
			TxChar(s, FS_SENTINEL);
		}
		TxFrSkyGPSStat(s); // 1
		//TxFrSkyCompassHeading(s);
	} else { // FRAME 1 every 200mS
		TxFrSkyBaro(s); 
		TxFrSkyVario(s); 
		TxFrSkyVoltage(s);
		TxFrSkyCellVoltages(s);
		TxFrSkyCurrent(s);
		TxFrSkymAH(s); 
		TxFrSkyGyro(s);
		// TxFrSkyAcc(s); // could add to check coordinated turns
		TxFrSkyAttitude(s);
		TxChar(s, FS_SENTINEL);
	}

	} // TxFrSkyTelemetry
	
### UAVX Setup ###

Load the telemetry version of the firmaware using the normal procedure. UAVX is then configured using UAVXGUI with only the telemetry port connected. You will need to calibrate the IMU and Magnetometer as usual. Configuration is as normal selecting UAVX telemetry (NOT FrSky) and arming by switch. Note: The arming switch needs to set to disarmed otherwise the board will switch to FrSky telemetry at 9600baud and not communicate with the GUI.

### Hardware Setup ###

Either V3 or V4 boards will work but the V3 board requires an external inverter on the Telemetry Tx line. A GPS is required!

The only signal connections to the board are the GPS, Telemetry Tx wire the Rx telemetry port providing ground and 5V (needed for the GPS). The RC inputs and the PWM outputs are not used. An optional current sensor may be connected. Power connections are flight battery to the board as usual. An Arming Switch is not needed and the input can be simply left open.

### Operation ###

Power up with the aircraft not moving - it does not need to be level. The UAVX initialisation proceeds as usual with the normal beeps etc (assuming you have a beeper connected) if all is well. Because the board is already armed it will attempt to obtain GPS lock. UAVX will not generate any telemetry at all until a good GPS signal is obtained. This can be detected as having occured when GPS data starts appearing at the Tx. You can have the GUI connected when commisioning the system with a temporary Arming Switch. You do not need to have the Arming Switch but bear in mind telemetry to the GUI will stop at the point that the GPS obtains lock and UAVX switches to FrSky telemetry and will stay there until disarmed or power cycled.
