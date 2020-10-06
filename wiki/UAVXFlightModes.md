# UAVX Navigation and Flight Modes #

UAVX has several modes most of which are associated with navigation and general control of the aircraft's attitude. It also has a large number of UAVXGUI checkboxes in the UAVXGUI.

Use the "BOOTLOAD" button when disarmed to place UAVX in bootloader mode. This removes the necessity of linking the boot pins.

## How are Modes Controlled ##

Some channel assignments deserve special mention. The first two are ALTERNATIVES and would normally use a potentiometer. AHNavSens ALWAYS overrides ArmAHNav therefore you should choose one or the other but not both.

  * ArmAHNav: Above 25% motors are ARMED. Above 50% altitude hold is enabled. Above 70% Waypoint navigation is enabled (See below). 
  * AHNavSens: Below 10% altitude hold is disabled.  Above 10% altitude hold is enabled and as you increase further the control action for position hold and waypoint navigation increase. You would normally use stick or switch ARMING if you use this and the Aux3 pin to enable or disable WP navigation. 
  
  * Fun/PH/RTH: The first position selects "Fun Fly" with all GPS navigation actions are disabled. In the middle position Position Hold is enabled. In the last position Return to Home is enabled. 
  * Ang/Hrz/Rate: Selects Angle, Horizon or Rate mode respectively (Google elsewhere). Nav/PH/RTH overrides this setting.
  * PassThru: disables angle and rate control completely in all aircraft except multicopters. Altitude hold is also disabled irrespective of the Nav/PH/RTH switch position.
 
You may reassign these functions to other channels should you desire.

There are several parameters and checkboxes associated with flight modes and navigation. UAVXGUI also describes their function in the text window.
 
  * Autoland: enables autolanding with multicopters (Fixed wing aircraft orbit the home position).
  * Use VRS Orbit: enables fast descents in all aircraft types. For multicopters this forces the aircraft to orbit while descending in an attempt to avoid VRS.
  * Turn towards Waypoint: for multicopters turns the aircraft towards the waypoint rather than holding heading.
  * Use Glider Strategies:  enables glider functions including boost then glide behaviours associated with thermal and other soaring (all modes associated with gliding are EXPERIMENTAL @ May 2017).
  * Fast Start: disables gyro recalibration at each launch and relies on values for gyro offset obtained during initial IMU calibration. Should be used for hand launced aircraft.
  * Manual AH: disables altitude hold in all modes including RTH - mainly used for testing and may be dangerous if aircraft is beyond line of sight in RTH.
  * Altitude Hold Alarm: counds and alrm on the beeper of altitude hold is not being maintained possibly due to thermal interference.
  * Beep at WP: Sounds Beeper whenever reaching a WP.
  * Use Home Ofset: The Home location is set after takeoff and the FW aircraft first reaches the Proximity Radius - or minimum turning radius of the aircraft.. 
  
The following are not directly associated with flight control:

  * Emulation: enables the emulation/simulation of aircraft in conjunction with UAVXGUI. The physics is **naive** so it should not be used for PID tuning.
  * Ext Inv: enables the use of inverted magnetometers integrated with some GPS units.
  * BLHeli: enables pass-through programming of BLHeli/SimonK ESCs when used in conjunction with BLHeliSuite.

## Startup Modes ##

UAVX has several initialisation steps which are displayed in UAVXGUI. These are not in order! Use the "Say Alarms" button to determine current alarms.

 * Starting (strt): Commencing arming startup sequence. If "Fast Start" checkbox is not checked then aicraft must be stationary. 
 * Warmup (wrm): Some sensors require a few seconds to stabilise so this is a short delay state. The baro is a strong syspect and so you should let the aircraft acclimitise to outside temperatures.
 * Landing (lndg): Aircraft is attempting to land.
 * Landed (down): Aircraft has landed but still armed.
 * Shutdown (shut): Aircraft has shutdown after a succesful landing or timeout. Should be disarmed.
 * InFlight (fly): Is currently in flight. FW aircraft once in flight can only be disarmed by power cycling as a a closed throttle is NOT a reliable way of detecting a landing. 
 * IREmulate (ir): currently unused.
 * Preflight (pre): Is conducting preflight checks. 
 * Ready (rdy): Has completed start up initialisation successfully but should still be disarmed.
 * ThrottleOpenCheck (thr): Checking that the throttle is closed.
 * ErectingGyros (gyro): initialising gyros i.e. calibrating for actual zero rate values - keep the aircraft still unless the "Fast Start" checkbox is checked.
 * MonitorInstruments (mon): not currently used.
 * InitialisingGPS (gps): Configuring the GPS.
	
## Navigation Modes ##

The modes or navigation states are displayed in UAVXGUI.

 * HoldingStation (hold): Aircraft is attempting to hold the GPS position captured before selecting this mode using the "Nav/RTH" switch.
 * ReturningHome (rth): Aircraft is attempting to return to the launch point.
 * AtHome (home): Aircraft is at or close to the launch point and will either orbit or land.
 * Descending (desc): Aircraft is attempting and autolanding.
 * Touchdown (down): Aircraft has landed.
 * Transiting (tran): Navigation is active and the aircraft is flying between waypoints.
 * Loitering (loit): Aircraft is holding position over a waypoint for the time set in the mission definition. For FW aircraft this means repeatedly attempting to cross the waypoint usually settling into some pattern.
 * OrbitingPOI (orbt): Aircraft is attempting to orbit a waypoint or point of interest at a radius set in the mission definition.
 * Perching (prch): Aircraft has landed and is waiting/perching for the time set in the mission definition before taking off and resuming the mission.
 * Takeoff (toff): Aircraft is taking off to a safe altitude before resuming the current mission.
 * PIC (pic): Pilot is in control with attitude stabilisation still active for multicopters but not FW aircraft.
 * AcquiringAltitude (acqa): Descending or climbing to desired altitude set by waypoint including Home.
 * UsingThermal (thrm): FW aircraft has detected a thermal and is attempting to exploit it.
 * UsingRidge (ridg): FW aircraft has detected ridge lift and is attempting to exploit it.
 * UsingWave (wave): FW aircraft has detected wave lift and is attempting to exploit it.
 * BoostClimb (bst): FW aircraft is using glider strategies and is boost climbing to a safe altitude from which to glide.
 * AltitudeLimiting (alim): FW aircraft has exceeded the maximum permissible altitude and is deploying spoilers and has cut throttle.
 * JustGliding (glid): FW aicraft is autonomously hunting for explotable lift.
 * WPAltFail (ALTF): Failure to reach WP desired altitude - possible thermal interference.
 * WPProximityFail (PRXF): Failure to reach WP - possible wind interference.

NOTE: Documentation is NEVER up to date with the firmware

