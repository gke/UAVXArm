# UAVX Navigation Controls #

Where national or controlling body regulations permit UAVX supports waypoint navigation and Return to Home (RTH/RTL) the latter which can be programmed to be a failsafe.

## Introduction

If a GPS is fitted then UAVX can perform return to home/launch (RTH) and waypoint navigation with missions loaded and/or viewed in the Navigation window of UAVXGUI.

## Configuration

There are several parameters associated with navigation functionality along with a number of overall behaviour qualifications in the form of checkboxes.

Tuning of the PID parameters is relatively straightforward but is conditioned in part by how good the GPS reception is.

 # xxxx

### Control Inputs ###

Two Rx channels are used as switches to control the navigation functions in flight. This functions are condition by what #Control Mode# has been selected.

 # Bypass: When activated this disables altitude hold and all navigation functions. For multicopters attitude hold (control mode is maintaind with either rate or angle control (selectable) The default Rx Channel for Bypass is 8. 
 # RTH/Nav: This requires a 3 position switch. 
  ## Low: Atitude control is active
  ## Medium:
 
 
  # Control Mode: This selects either rate control where sticks control rotation rate or angle control where sticks control attitude angle. Yaw is always rate control. If the sticks are centred then attitude angle control overrides rate control with the #Horizon# parameter being used to determine at what percentage of stick movement control transitions to Rate Control. The default channel for control mode is 6.

  ### PID Parameters

The gains for the inner pitch/roll/yaw and altitude rate loops will be much lower than for multicopters. 
Typically half for Pi and Di. The Di parameter can cause a lot of servo jitter so reducing these further may help if you are not flying an unstable flying wing that is.

The gains for the outer setpoint loops (Po) will be similar to those used for multicopters.

### Arming and Accelerometer Trim

When calibrating it is best to orient the aircraft in approximately the pose for best glide which in most cases be slightly nose up. 

Arming is in the normal way but you will not have **stick** based **disarming available**. The reason is that in flight, unlike multicopters, you may well have zero throttle at times when gliding for example. It is therefore possible to disarm the aircraft accidentally. It will be necessary therefore to power recycle the aircraft if you wish to disarm.

With **switch** based arming you can disarm on landing using the switch in the normal way.

Accelerometer trim using sticks is available only when armed and after landing. The complication is that you will now only be able to do trimming once immediately after powering up and arming. Once you have taken to the air you will need to **power cycle** on the ground and re-arm to gain access again to the trims.

As trimming is only required when commissioning a new aircraft this should not be a major inconvenience.

### Mixing

You should use Tx setting with no mixing and **no trims** as for multicopters. 
Any throws and exponential settings on the Tx are used however and need in some cases to have sufficient throw for stick based arming and accelerometer trim tuning. This may be achieved by using full throws and increased exponential in most cases of where dual rates are available use those.

UAVX uses a consistent internal represention for roll/pitch and yaw. It is important to check that the control surfaces react correctly to UAVX first then reverse the control sense of Tx channels where necessary.

If the servo sense/direction is incorrect you may reverse it using the boxes in the RC parameter group in the GUI Parameters Window. The help window will tell you which of the six "servos" you are changing. 
The order of the boxes is Right Aileron/Elevon, Left Aileron/Elevon, Elevator, Rudder, Right Flap, Left Flap. 

Remember parameters on the aircraft do not directly track, or match, any changes in the GUI. You must select WRITE for this to happen.

So for a conventional aileron aircraft this is all pretty much obvious so what about a pure Elevon Flying Wing model with no rudder or elevator. In this case there is no servo to reverse for either the rudder or elevator. Nonetheless the process is the same, as reversing the elevator "servo" just tells the UAVX mixing to reverse the sign of the elevator value before adding it to the elevon value to get the total deflection. Straightfowrard.

### Output Channel Assignments

The order of the output channel assignments is generally:

* M1 Throttle
* M2 Right Aileron/Elevon
* M3 Left Aileron/Elevon
* M4 Elevator
* M5 Rudder
* M6 Right Flap
* M7 Left Flap (this is the reversed value of M6)

If a parallel Rx connection is used then M7..M10 are unavailable as RX8..RX5 uses the same pins as M7..M10; note the order. This means that to use Left Flap directly you may need to have a servo reverser as usually the flap servos are opposite handed like the ailerons. If the flaps are driven from a simple torque rod then you only need one servo channel.

UAVX may not compute values for all outputs depending on the aircraft and the aircraft may not necessarily use all that are computed. Examples include:

* Wing: Throttle, Right Elevon, Left Elevon
* Delta: Throttle, Right & Left Elevon, Rudder and optionaly Right and Left Flap

UAVX computes roll/pitch and yaw commands and these are mixed into the control surface motions as you would expect logically :). If you use a Delta as a Flying Wing and do not connect a rudder then there is no yaw control so it if it looks like a Delta but has no rudder on the fin then it is a Flying Wing. Yes looks can be deceiving ;).
       
## TODOS

Yes it is not as sophisticated/complicated as some but works in most cases. ToDos: 

 * The current mixing is not general and could be improved to allow custom mixing formulae but maybe later.
 * Although aileron differential is in the code, and camber etc associated with gliders may be added, the number of parameters will have to be increased from the UAVP legacy 64.
