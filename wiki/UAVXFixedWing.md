# UAVX Fixed Wing Operation #

UAVX supports aircraft other than pure multicopters. It is not currently as sophisticated as some in that it does not claim autoland or full aerobatic capabilities. It does however provide both angle 
and rate stabilisation and a failsafe return to home (RTH) capability using GPS. Where regulations permit it also supports waypoint navigation.

## Introduction

It is assumed you have read the UAVXGUI and UAVXArm32F4Startup wikis.

For non-multicopter aircraft you have the ability to **bypass** almost all of the UAVX functionality except the final mixing. By default **bypass** is assigned to Rx channel 8 but may be reassigned using the GUI. If you find that you do not have enough channels to assign one to bypass the default is **bypass disabled** so stabilisation is always on. 

There only a few parameters specific to non-multicopter aircraft including fixed wing aircraft.

The RollPitchMix parameter provides anticipatory, or feedforward, up-elevator in rolls and should be used carefully. Perhaps start with 20%.

The Horizon parameter controls determines at what stick deflection the aircraft goes from rate control to direct control (i.e. no stabilisation)
rate control mode is selected. 

The default RTH Alt parameter is probably too low and could be doubled. When RTH is engaged the aircraft will return and **orbit** at this altitude, not hover obviously, so it needs to be clear of obstacles like trees, flag poles and buildings.
 
**IMPORTANT**: By now you should know that any programming of flight controllers should be done with props OFF. Some changes to the aircraft type, Rx type/mode or Emulation mode etc. may require a reset so that the hardware can be reconfigured SAFELY. If there is no response to UAVXGUI commands then check the LEDs to see if they are all flashing. If they are then power cycle the board only after you have checked 
that your aircraft matches the one selected. 

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
