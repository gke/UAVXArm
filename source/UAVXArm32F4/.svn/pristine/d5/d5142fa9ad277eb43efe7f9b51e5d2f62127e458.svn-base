# UAVX Failsafe Operation #

UAVX monitors the signals from the Rx to UAVX **NOT** from the Tx to the Rx.

UAVX requires the signals from the Rx to meet minimum and maximum pulse width requirements which mean the endpoint adjustments (EPAs) for each channel must be adjusted properly - see the UAVXSetup Wiki.

## Modern Receivers with Failsafe ##

Modern receivers usually maintain sensible channel values when the link between the Tx and the Rx fails for some reason.

### Rx Failsafes not set correctly ###

If you have not set failsafe values for the Rx and adjusted the EPAs so they too pass the Rx test in UAVPSet the behaviour will be unpredicable. If your EPAs violate the UAVX setup limits then it likely the Rx packets will be rejected as bad and the behaviour will be as for older non-failsafe receivers (see Older Receivers below).

### How do I set the Rx Failsafes ###

You should set  the failsafes so that roll/pitch and yaw go to neutral, throttle goes to well under hover (1/2 - 2/3) and Channel 5 selects RTH. Nav sensitivity should be what you have determined to work well for position hold and the camera trim should be so your camera is tilted to a safe position. The amount of throttle required to achieve hover obviously depends on any changes in aircraft weight, battery condition, changes to propellors, motors etc and  also on the **weather**.

It is very important to note that the aircraft will do exactly what the Rx settings tell it to do. If you have programmed the Rx hold last setting then that is what UAVX will do - all the way **into the ground** in a NASA styled "controlled impact with terrain" or **up up and away** if the throttle is too high and it will continue to do this until the fact that the sticks are not changing is detected ( see No GPS ). If you selected Channel 5 to be RTH as suggested there will be no delay and and the aircraft should return to the launch point at the desired altitude.

### What Happens with no GPS ###

If you do not have GPS installed and operating  with the origin set then and the Rx goes to failsafe then this can normally be detected by UAVX **as the controls do not change**, which they would do when controlling the aircraft without position hold.  If the controls are not moved for 20 seconds then the aircraft will sound a **failsafe alarm** which at about **2 beeps per second** and attempt to land. When it believes it has landed it sounds the **lost model alarm** (See Lost Model).

If it is not actually a real Rx failsafe and you were simply not moving the sticks then moving them cancels the UAVX failsafe but only while you are still hearing the **failsafe alarm**.

### What happens with GPS ###

If your GPS is **installed**, working **and** the Origin recorded the aircraft should return to the origin and, with no further intervention, attempt to land after the time delay set in UAVPSet. You MUST have RTH set as your Rx failsafe if you expect the aircraft to return. If you do not then the aircraft will hold position until its battery runs out at which time it will land/crash. It is simply to difficult to resolve between an aircraft navigating or holding position with no control input or a genuine Rx failsafe.

## Older Receivers ##

With older PPM style receivers the signal from the Rx becomes just noise if the Tx link fails.

If the Rx signal fails and does not fully recover within 2 seconds then the aircraft will attempt to perform a RTH if GPS is set up properly or if GPS is not available land where it is sound the **failsafe alarm** followed by the **lost model alarm** when it has landed.

## Lost Model Alarm ##

If your aircraft is armed but not flying ( the throttle has closed or the Rx failsafe throttle setting is closed) and the sticks have not moved for 2-3 minutes the **lost model alarm** will sound.  The alarm is a **beep every 6 seconds**. This is independent of throttle setting and whether your Tx is on or off and may be of use if your aircraft goes down and is difficult to locate. The lost model alarm can only be reset by doing a **disarm/arm cycle**.

## Flight Statistics ##

Routinely check your flight (X stats test) after flights to see if there have been more than a handful of **glitches** or whether there have been any other sensor failures. The stats are kept from when you disarm to when re-arm even if the battery is unplugged so you can check this at home if that is where your computer is as long as you do not re-arm.