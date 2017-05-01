# UAVX Flight Modes #

UAVX has several flight modes most of which are associated with navigation. These are displayed on UAVXGUI and MavLink telemetry typically inflight on minimOSD.

## How are Modes Controlled ##

There are three channels which condition these modes:

 * "Nav/RTH" switch defaulting to a three position switch Ch5 . The first position is pilot in control (PIC), the second activates position hold (HoldingStation) or waypoint navigation and the third is always return to home (RTH). 
 * "Ctrl. Mode" defaulting to channel 6 selects between angle or rate control. With angle control the aircraft's pitch/roll angle is proportional to stick deflection. With rate control the aircraft's roll/pitch rate is proportional to stick deflection. The transition point from angle control to rate control can be further qualified using the "Horizon" parameter. 
  * "Bypass" defaulting to channel 8 allows you to bypass both angle and rate control completely in all aircraft except multicopters. 

## Modes ##

The modes displayed in UAVXGUI and on a (minimOSD) are then:

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
 * AltitudeLimiting (alim): FW aircraft has exceeded the maximum permissible altitude and is deploying spoilers and has cut thorttle.
 * JustGliding (glid): FW aicraft is autonomously hunting for explotable lift.
 * RateControl (rate): Aircraft roll pitch and yaw rates are proportional to Tx stick deflection. Autonomous navigation is not available in this mode however selecting RTH/RTL will force the aircraft to attititude or angle control. Also referred to as Acro mode.
 * BypassControl (man): Configuration only mixing is applied to the Rx commands and routed directly to the throttle and control surfaces i.e. direct MANUAL control.


