# UAVX Flight Modes #

UAVX has several flight modes most of which are associated with navigation and general control of the aircraft's attitude. These are displayed on UAVXGUI and MavLink telemetry typically inflight on minimOSD.

## How are Modes Controlled ##

There are three channels which condition these modes. Once initiated the aircraft will move to other modes as required. The three channels below override any intermediate modes. For example RTH will usually trigger a sequence of modes including Transiting, AtHome, Loitering, Descending and Touchdown. "Bypass" however is meant to immediately give full control of the aircraft to the pilot. 

 * The "Nav/RTH" three position switch and defaults to Ch5 . The first position is pilot in control (PIC), the second activates position hold (HoldingStation) or waypoint navigation (involving several navigation states/modes) and the third is always return to home (RTH). Altitude hold is active in all switch positions if the throttle has not moved for several seconds and the rate of climb/descent is sufficiently low (typically <1M/S). If you don't have a GPS and a valid home position recorded then all switch positions give PIC (see Capture of the Home or Launch Position below).
 * The "Ctrl. Mode" switch, defaulting to channel 6, selects between angle or rate control. With angle control the aircraft's pitch/roll angle is proportional to stick deflection. With rate control the aircraft's roll/pitch rate is proportional to stick deflection. The transition point from angle control to rate control can be further qualified using the "Horizon" parameter. Rate control is overriden if the Nav/RTH switch is in any position other than PIC. The selected angle/rate control is restored if the Nav/RTH switch returns to PIC. 
  * The "Bypass" switch, defaulting to channel 8, allows you to bypass both angle and rate control completely in all aircraft except multicopters. Altitude hold is disabled irrespective of the Nav/RTH switch position.
 
You may reassign these functions to other channels should you desire.

You may choose to use the "Nav Sens" channel (default Ch7) to reduce the Pvel in flight. Pvel controls the pitch/roll angle for a given position error. The minimimum value is 25% and the maximum 1--%. If you do not use this channel then the default is 100% which means Pvel is used directly. 

There are several parameters and checkboxes associated with flight modes and navigation. It is important to note the functions when checked of the following checkboxes.
 
  * Failsafes: enables all failsafe functions. 
  * Req. GPS: enables the requirement for a good GPS signal before permitting throttle up. 
  * Autoland: enables autolanding with multicopters (Fixed wing aircraft orbit the home position).
  * Fast Desc: enables fast descents in all aircraft types. For multicopters this forces the aircraft to orbit while descending in an attempt to avoid VRS.
  * Glider:  enables glider functions including boost then glide behaviours associated with thermal and other soaring (all modes associated with gliding are EXPERIMENTAL @ May 2017).
  * Fast Start: disables gyro recalibration at each launch and relies on values for gyro offset obtained during initial IMU calibration. Should be used for hand launced aircraft.
  
The following are not directly associated with flight control:

  * Emulation: enables the emulation/simulation of aircraft in conjunction with UAVXGUI. The physics is **naive** so it should not be used for PID tuning.
  * Reboot: enables auto reboot of the flight controller (FC) after writing parameters which change the aircraft configuration. This avoids the need to power cycle the FC.
  * Mag Inv: enables the use of magnetometers integrated with some GPS units.
  * Brd Inv: enables the FC to be mounted upside down in the aircraft.
  * LEDs: enables WS2812 type multi colour LED displays.
  * BLHeli: enables pass-through programming of BLHeli/SimonK ESCs when used in conjunction with BLHeliSuite. 

## Capture of the Home or Launch Position ##

This only applies if you have a GPS fitted. 

If you do not select "Req. GPS" the home position will be wherever the aircraft is located when you FIRST select other than PIC using the Nav/RTH switch. The intention is of course to allow you to fly the aircraft out to a convenient position in front of you and record a launch point there to avoid having it return on an RTH to orbit over the top of yourself and other flyers which can be very annoying.

If you select the "Req. GPS" checkbox you will not be able to throttle up, even if armed, until an acceptable GPS position has been obtained to record the home or launch location. You should arm to enable the acquisition of the home position when you are positioned at the desired launch point. The home position will normally be reacquired when you return to the Landed mode. A single longish beep will be heard if you have a beeper when home is acquired replacing the eight beeps previously.

The choice not to use "Req. GPS" is entirely at your own risk as there is the distinct possibility of forgetting to record a launch point. In this case you could be a long way away and select RTH!!!!  The aircraft will capture the current position as the launch point and do a RTH by descending right there which may be anywhere. 


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


