# UAVX Fixed Wing Operation DRAFT @ February 2017 #

UAVX supports aircraft other than pure multicopters. It is not currently as sophisticated as some in that it does not claim autoland or full aerobatic capabilities. It does however provide both angle 
and rate stabilisation and a failsafe return to home (RTH) capability using GPS. Where regulations permit it also supports waypoint navigation.

## Parameters ##

It is assumed you have read the UAVXGUI wiki.

The most important function is the ability to **bypass** almost all of the UAVX functionality except the final mixing. By default **bypass** is assigned to Rx channel 8 but may be reassigned using the GUI. 

There only a few parameters specific to non-multicopter aircraft including fixed wing aircraft.

The RollPitchMix parameter provides anticipatory, or feedforward, up-elevator in rolls and should be used carefully. Perhaps start with 20%.

The Horizon parameter controls determines at what stick deflection the aircraft goes to full rate control behaviour when 
rate control mode is selected. This could be reduced to say 5%.

The default RTH Alt parameter is probably too low and could be doubled. When RTH is engaged the aircraft will return and **orbit** at this altitude, not hover obviously, so it needs to be clear of obstacles like trees, flag poles and buildings.
 
IMPORTANT: By now you should know that any programming of aircraft should be done with props OFF. Some changes to the aircraft type, Rx type/mode or Emulation mode etc. may require a reset so that the hardware can be reconfigured SAFELY. 
Take great care not to select any of the non-multicopter airframe types when programming parameters for multicopters. The default state, or setting, for servos when powering up is mid position which is **HALF THROTTLE** on motors.
If there is no response to UAVXGUI commands then check the LEDs to see if they are all flashing. If they are then power cycle the board only after you have checked 
that your aircraft matches the one selected. 

## PID Parameters ## 

The gains for the inner pitch/roll/yaw and altitude rate loops will be much lower than for multicopters. 
Typically half for Pi and Di. The Di parameter can cause a lot of servo jitter so reducing these further may help if you are not flying an unstable flying wing that is.

The gains for the outer setpoint loops (Po) will be similar to those used for multicopters.

### Mixing ###

You should use Tx setting with no mixing and **no trims** as for multicopters. 
Any throws and exponential settings on the Tx are used however and need in some cases to have sufficient throw for stick based arming and accelerometer trim tuning. This may be achieved by using full throws and increased exponential in most cases of where dual rates are available use those.

UAVX uses a consistent internal represention for roll/pitch and yaw. It is important to check that the control surfaces react correctly to UAVX first then reverse the control sense of Tx channels where necessary.

If the servo sense/direction is incorrect you may reverse it using the boxes in the RC parameter group in the GUI Parameters Window. The help window will tell you which of the six "servos" you are changing. 
The order of the boxes is Throttle, Right Aileron/Elevon, Left Aileron/Elevon, Elevator, Rudder, Flap. The Throttle reverse is inactive. 

Remember parameters on the aircraft do not directly track, or match, any changes in the GUI. You must select WRITE for this to happen.

So for a conventional aileron aircraft this is all pretty much obvious so what about a pure Elevon Flying Wing model with no rudder or elevator. In this case there is no servo to reverse for either the rudder or elevator. Nonetheless the process is the same, as reversing the elevator "servo" just tells the UAVX mixing to reverse the sign of the elevator value before adding it to the elevon value to get the total deflection. Straightfowrard.

       
## TODOS ##

Yes it is not as sophisticated/complicated as some but works in most cases. ToDos: 

 * The current mixing is not general and could be improved to allow custom mixing formulae but maybe later.
 * For those with 7 Channel Rx I should have Bypass default to off or inactive. 


