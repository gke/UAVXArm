# Introduction #

Full telemetry is available at all times regardless of whether the aircraft is armed or not except where you use the main COM port for GPS - see below.

You should also refer to the Wikis for the old separate tools.

The actual layout of the windows changes over time and will not match those used as examples here.

## Voice Feedback ##

Selectable voice feedback is available for those who like to look at the aircraft in flight rather than hopefully where it is on a screen. 

# Groundstation #

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUIMain.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUIMain.JPG)

The main Ground Station window has the main instrumentation and replaces the test functions of UAVXSet.

The buttons to the top right select the Parameter and navigation windows. Magnetometer and IMU (Gyros/Accs) calibration are below the Flags Box.  They are click to commence and are reset automatically (Green) when calibration is complete. If they are Red then the sensors are uncalibrated. This group of buttons also includes Spektrum and SBus loopback tests. All of these buttons are available only when disarmed.

# Parameters #

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUIParams.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUIParams.JPG)

Open this page using the button top right of the Ground Station page. You may close and reopen this page as you desire.

There is now only one page for parameters with the parameter set number being changed using the selector in the toolbar. As with UAVXSet the Read and Write buttons must be used to update the aircraft's values. Currently there are two parameter sets available but there is no real limit.

Parameter sets may also be saved or loaded from file as required.

IMPORTANT: By now you should know that any programming of aircraft should be done with props OFF. Some changes to the aircraft type, Rx type/mode or Emulation mode etc. may require a reset so that the hardware can be reconfigured SAFELY. 
Take great care not to select any of the non-multicopter airframe types when programming parameters for multicopters. The default state, or setting, 
for servos when powering up is mid position which is **HALF THROTTLE** on motors.
If there is no response to UAVXGUI commands then check the LEDs to see if they are all flashing. If they are then power cycle the board only after you have checked 
that your aircraft matches the one selected. 

## A Reminder on Failsafes ##

UAVX will do whatever you have set on your Rx failsafes. Carefully set them to the action you desire. Usually this is sticks neutral and RTH selected.

# Navigation #

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUINav.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/UAVXGUINav.JPG)

Open this page using the button top right of the Ground Station page. You may close and reopen this page as you desire.

Navigation including hold is disabled for the first approximately 15 seconds of flight.  Selecting Nav Mode other than PIC will result in an autoland. You can abort this by reversing the switch action. This has been adopted to reduce the possibility of engaging waypoint navigation while standing close to or over the aircraft immediately after arming.

## General ##

Missions may be accessed as files using the Load and Save buttons. An image of the current map is also saved when using the Save button.

Changes to the mission on the map must be uploaded to the aircraft using the Write button. The current mission stored in the aircraft (if any) is downloaded using the Read button.

## Emulation ##

You should use **Emulation Mode** to help you become familiar with the mission planning and the effect of the various Nav Modes (Channel 5 switch positions).

You select Emulation Mode using the Flight Mode Checkbox. UAVX will then use representative physics to display what the aircraft is likely to do in flight; this includes GPS.  The emulation has a light breeze blowing to the NE and you will see the effect of this.

To get you started Emulation Mode generates a representative mission plan which you may change. This mission is currently **NOT DELETED** when you switch out of Emulation mode and maybe deleted by uploading an empty mission (no waypoints) using the Write button.

**REMOVE YOUR PROPELLORS WHEN USING EMULATION**. The software should disarm motor function when emulation is selected but I don't trust software so neither should you.

## First Emulated Flight ##

Select the Read button or select  Read in the mouse right click menu. This should upload any mission currently stored in UAVX. In Emulation Mode you should see a simple mission displayed.

**WAIT** until the "Home Set" status box is lit signifying GPS lock has occurred and the origin has been acquired.

Increase throttle and "fly" to around 15M. Slowly reduce the throttle to 45% which is hover. You should see the ROC drop to close to zero at which point the altitude hold will engage.

Select Navigate on Channel 5. You should now see (and hear) the aircraft start to traverse the mission.

Try selecting RTH then re-selecting Navigate. Note you must go to Hold then back to Navigate otherwise RTH will continue.  Currently the mission is restarted not resumed where it left off.

## More Advanced ##

Most of the usual features you would expect are available:

  * The types of waypoints and some edits are available with right mouse click - the position of the mouse when you click will be the position of the waypoint.
  * The up/down buttons in the mission data box may be used to edit default values, change the waypoint sequence or delete individual waypoints.
  * Conditional waypoint sequences/loops are not currently available.

The waypoint types are currently:

  * VIA - where the aircraft pauses (T) at a waypoint attempting to stay within the proximity radius and altitude limits. If the time is set to zero then the aircraft must go through the waypoint within the proximity radius but not necessarily the altitude.
  * ORBIT - orbits the waypoint at the radius (R) with a velocity (V). UAVX will point an camera at the waypoint.
  * PERCH - lands at the waypoint then takes off and resumes mission.
  * Set POI - this is not a waypoint as such just a point that the aircraft will point to for the rest of the mission or until another Set POI is encountered.
  * SURVEY - this allows you emit a pulse of a fixed width on the Aux1 pin repeated at periodically commencing at the next waypoint.
  
Although not displayed RTH is always the default last waypoint. If there is no mission then selecting Navigate results in RTH.

If you do not designate a POI then the normal Turn To WP or hold heading configuration applies.

If the aircraft is currently at a much higher altitude than that desired upon reaching a waypoint it is possible to achieve a much mre rapid descent.  Rapid vertical descents in rotorcraft can result in a condition called Vortex Ring State (VRS). We attempt to avoid this condition by forcing the aircraft into forward orbiting flight until it close to the desired altitude at which time the descent rate will will slow to that set in the parameters.  This strategy is enabled using the "Normal/Rapid Checkbox. If you are flying manually you should have a small amount of forward motion when descending rapidly otherwise you may just go all the way in! The same effect can occur if you just go quickly to maximum throttle when carrying a heavy camera load.

http://en.wikipedia.org/wiki/Vortex_ring_state

Any changes to the mission do not become effective until you **select Write**. You should always select Read immediately after to satisfy yourself that the mission appears OK.

Note: UAVXSet also allows you to check whether a current mission is loaded in the **Setup Test**.

## Go To ##

If the Go To checkbox is checked then a right click of the mouse will generate a 1 point mission. This allows you to fly to a designated point dynamically with no control inputs. Again you must select Write to upload the mission change.

## Finally ##

It is important to familiarise yourself with what happens when you change the Channel 5 switch selection. Some of the changes may not be exactly as you expect and it is better to see the effect under emulation ;).
