## Stick Programming ##

The Tx sticks may be used to change some parameters or adjust the accelerometer neutrals in the field.

The blue LED will flash accompanied by the beeper when you have changed a setting. You have to hold the stick for 2 seconds before this happens and the change is accepted.

### Arming and Disarming ###

Normally you would arm/disarm using the arming switch. By selecting the appropriate checkbox in UAVPSet you can also enable arming/disarming using the **Aileron** or **Rudder** stick. For safety the arming switch has to set to arm to enable stick arming.

### When Disarmed ###

Elevator stick down Rudder stick left parameter set 1 and right parameter set 2.

### When Armed ###

It is inevitable that the accelerometer neutrals will require small adjustments regardless of how well you leveled the aircraft during setup.

You should **NOT** use the Tx trims which should be set to neutral (no trims). You can do this using the Params Window in UAVXGUI. Choose a day when there is no wind. Remember you only need to do this once unless you have a major crash so it worth doing it carefully as there is very little drift in the accelerometer neutral values.

  * fly the aircraft pointing away from you and note the directions that it drifts.
  * land and, with the throttle closed but still armed, hold the **Rudder** or **Elevator** stick at maximum in direction opposite to the drift until you hear a beep.
  * Fly again repeating until there is no drift.

If you continue to hold the stick at the extreme after the first LED flash and beep, the change will be repeated every quarter of a second accompanied by another beep and flash.
