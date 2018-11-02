# UAVX Flight Modes #

UAVX has several flight modes most of which are associated with navigation and general control of the aircraft's attitude. These are displayed on UAVXGUI and MavLink telemetry typically inflight on minimOSD.

## How are Modes Controlled ##

Some channel assignments deserve special mention. The first two are ALTERNATIVES and would normally use a potentiometer. 

  * ArmAHNav: Above 25% motors are ARMED. Above 50% altitude hold is enabled. Above 75% Waypoint navigation is enabled (See below). 
  * AHNavSens: Below 10% altitude hold and is disabled.  Above 10% altitude hold is enabled and as you increase further the control action for position hold and waypoint navigation increase.
  
  * Fun/PH/RTH: The first position selects "Fun Fly" with all GPS navigation actions are disabled. In the middle position Position Hold is enabled. In the last position Return to Home is enabled. 
  * Ang/Hrz/Rate: Selects Angle, Horizon or Rate mode respectively (Google elsewhere).   
  * PassThru: disables angle and rate control completely in all aircraft except multicopters. Altitude hold is also disabled irrespective of the Nav/RTH switch position.
 
You may reassign these functions to other channels should you desire.

You may choose to use the "Nav Sens" channel (default Ch7) to reduce the Pvel in flight. Pvel controls the pitch/roll angle for a given position error. The minimimum value is 25% and the maximum 1--%. If you do not use this channel then the default is 100% which means Pvel is used directly. 

There are several parameters and checkboxes associated with flight modes and navigation. It is important to note the functions when checked of the following checkboxes.
 
  * GPS Required: enables the requirement for a good GPS signal before permitting throttle up. 
  * Autoland: enables autolanding with multicopters (Fixed wing aircraft orbit the home position).
  * Fast Descent(VRS): enables fast descents in all aircraft types. For multicopters this forces the aircraft to orbit while descending in an attempt to avoid VRS.
  * Turn towards Waypoint: for multicopters turns the aircraft towards the waypoint rather than holding heading.
  * Use Glider Strategies:  enables glider functions including boost then glide behaviours associated with thermal and other soaring (all modes associated with gliding are EXPERIMENTAL @ May 2017).
  * Fast Start: disables gyro recalibration at each launch and relies on values for gyro offset obtained during initial IMU calibration. Should be used for hand launced aircraft.
  * Manual AH: disables altitude hold in all modes including RTH - mainly used for testing and may be dangerous if aircraft is beyond line of sight in RTH.
  
The following are not directly associated with flight control:

  * Emulation: enables the emulation/simulation of aircraft in conjunction with UAVXGUI. The physics is **naive** so it should not be used for PID tuning.
  * Reboot: enables auto reboot of the flight controller (FC) after writing parameters which change the aircraft configuration. This avoids the need to power cycle the FC.
  * Mag Inv: enables the use of magnetometers integrated with some GPS units.
  * Brd Inv: enables the FC to be mounted upside down in the aircraft.
  * #LEDs: if greater than zero enables WS2812 type multi colour LED displays.
  * BLHeli: enables pass-through programming of BLHeli/SimonK ESCs when used in conjunction with BLHeliSuite.
  
  * BOOTLOAD: Equivalent to power cycling for loading new firmware. The link/switch on the bootpins should not be used unless the board is not responding to GUI commands.  

## Modes ##

The modes or navigation states are displayed in UAVXGUI and on a (minimOSD).

 * HoldingStation (hold): Aircraft is attempting to hold the GPS position captured before selecting this mode using the "Nav/RTH" switch.
 * ReturningHome (rth): Aircraft is attempting to return to the launch point.
 * AtHome (home): Aircraft is at or close to the launch point and will either orbit or land.
 * Descending (desc): Aircraft is attempting and autolanding.
 * Touchdown (down): Aircraft has landed.
 * Transiting (tran): Navigation is active and the aircraft is flying between waypoints.
 * Loitering (loit): Aircraft is holding position over a waypoint for the time set in the mission definition. For FW aircraft this means repeatedly attempting to cross the waypoint usually settling into some pattern.
 * OrbitingPOI (circ): Aircraft is attempting to orbit a waypoint or point of interest at a radius set in the mission definition.
 * Perching (prch): Aircraft has landed and is waiting/perching for the time set in the mission definition before taking off and resuming the mission.
 * Takeoff (toff): Aircraft is taking off to a safe altitude before resuming the current mission.
 * PIC (stab): Pilot is in control with attitude stabilisation active.
 * AcquiringAltitude (acqa): Descending to desired altitude set by waypoint including Home.
 * UsingThermal (thrm): FW aircraft has detected a thermal and is attempting to exploit it.
 * UsingRidge (ridg): FW aircraft has detected ridge lift and is attempting to exploit it.
 * UsingWave (wave): FW aircraft has detected wave lift and is attempting to exploit it.
 * BoostClimb (bst): FW aircraft is using glider strategies and is boost climbing to a safe altitude from which to glide.
 * AltitudeLimiting (alim): FW aircraft has exceeded the maximum permissible altitude and is deploying spoilers and has cut throttle.
 * JustGliding (glid): FW aicraft is autonomously hunting for explotable lift.
 * RateControl (rate): Aircraft roll pitch and yaw rates are proportional to Tx stick deflection. Autonomous navigation is not available in this mode however selecting RTH/RTL will force the aircraft to attititude or angle control. Also referred to as Acro mode.
 * BypassControl (man): Configuration only mixing is applied to the Rx commands and routed directly to the throttle and control surfaces i.e. direct MANUAL control.


