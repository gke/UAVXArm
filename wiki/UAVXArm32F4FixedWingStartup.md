## Introduction ##

**DEFINITELY UNDER CONSTRUCTION**

Read the UAVXArm32F4Startup Wiki first. This Wiki contains supplementary notes specific to using UAVXArm32F4 on a fixed wing aircraft.

UAVX has not been regularly used for fixed wing aircraft so congratulations this is new territory and makes you one of the first Alpha Testers.

#### UAVXArm32F4 Board ####

![https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVXArm32F4Connections.png](https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVXArm32F4Connections.png)

### Step 1 ###

Mount the board as near as possible to the flight level angle with the ARROW on the top of the board must point directly forward.

Take all the the shorting links (if any) off the board. Connect up an arming switch between the Arming pin and the adjacent Ground pin.

The motor should not run even if you have the arming switch on until you get through all of the following steps but take the prop OFF or disconnect the motor leads.

The board can be connected directly to your main battery (3S LiPo).

The Red light should be on and the Yellow LED should be flashing once a second indicating that you have not yet calibrated your accelerometers.

### Step 2 ###

Download the software package from the downloads area and install UAVPset and UAVXGS.

Connect the adapter lead to the board and then to a USB port on your computer.

![https://github.com/gke/uavp-mods/blob/uavxarm32_graphics/ftdi.jpg](https://github.com/gke/uavp-mods/blob/uavxarm32_graphics/ftdi.jpg)

Start UAVPSet and check that you have the appropriate COM port selected.

Pulldown the Tools menu and select Software Test. You will see a range of tests.

In the tools select Load defaults.  Close the Software Tests window and select the **Read Config** icon. The default parameter set should populate the various boxes.

There are a lot of parameters most of which you may never have to adjust. If you highlight any of the parameter boxes explanatory text should be displayed in the text window.

The default parameters you first see will be for multicopters not fixed wing aircraft so you need to do the following.  Some parameter changes involve an electrical reconfiguration of the board so you will occasionally see all LEDs flashing - cycle the power and this should allow the board to complete the reconfiguration.

  * disconnect servos as the motor/servo ports will initially be being updated at 450Hz way too high for most servos.
  * power up and select your desired fixed wing airframe
  * load the defaults which are a first guess at what they may be..
  * connect the servos

It is important to note that unlike the multicopter default parameters which were obtained from flights by Ken & Jim using quadrocopters we do not yet have a sensible starting set of default parameters for fixed wing aircraft. You will definitely need to tune the Roll/Pitch and Yaw parameters in particular to suit your own flying style and the size/span and weight of your aircraft.

As you tune these parameters you can save the results to a parameter file which you can re-read. It is a good idea to periodically save your parameter sets.

Immediately change the aircraft pulldown to the fixed wing aircraft type you are using.

From now on any changes you make to parameters in UAVPSet must be followed by selecting the **Write Config** icon.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_Params_Wing.JPG

Now select the Setup Test.  Read the results and try to make sense of it. At the bottom you will see various alarms. With luck the only alarms will be that you have not calibrated the accelerometer and the Rx is not working.

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVPSet_Start.JPG

### Step 3 (Rx) ###

Important - the black or ground pins of your Rx and motor and servo connections should be connected to the pins closest to the edge of the board. If in doubt double check with a multimeter.

Connect the motor throttle control lead to K1. This will power your Rx as centre pins of all of the M and Rx pins are connected together.

If you have a a Rx (e.g. FrSky) that supports Compound PPM (CPPM) connect it to Rx1. For CPPM you must specify the number of channels your Rx is receiving using UAVPSet.

If you have an Rx that uses parallel PPM then connect it to Rx1 up to Rx8 in the order Throttle, Aileron, Elevator, Rudder, Gear, Aux1, Aux2, Aux3. You need a minimum of 7 channels for full functionality.

For parallel PPM using UAVPSet set number of channels you have actually connected.

You can fly with a minimum of 4 channels Throttle, Aileron, Elevator, Rudder. If you do so then you will have altitude hold but no navigation capability.

You may re-assign the channels using the selectors in UAVPSet. So if your throttle is on Channel 3 for example then setup the pulldowns appropriately.

Turn on your Tx and run the Setup Test again. If you got lucky the Rx will be OK. Select the Rx Test and it will show you which channels are failing. The throttle channel should be around 5%, Aileron, Elevator and Rudder about 50% and Gear around 10%.

http://uavp-mods.googlecode.com/svn/branches/uavx_graphics/UAVPSet_RxTest.JPG

If the Rx Test passes then close the test software window and select the Tx/Rx setup icon. Double check the controls are behaving sensibly and the endpoints are under 100%.

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVPSet_RxGraphic.JPG

As well as adjusting the navigation sensitivity NavS or Channel 7 (normally a potentiometer/knob) is used to enable altitude hold.  Initially you should fly with altitude hold off. Altitude hold is switch OFF below 10% and ON above 10%. The knob does not control altitude hold sensitivity.

At this point there is no automatic control of the surfaces which is enabled only after arming.

The Green LED should be on and Yellow LED flashing every second.

### Step 4 (Acc/Gyro Temperature Calibration) ###

This is the most critical step in the setup and you usually only need to do it once or until you next bend the aircraft.

**YOU MUST CALIBRATE ACCELEROMETERS OTHERWISE THE ATTITUDE ESTIMATES (PITCH/ROLL ANGLES) WILL BE INCORRECT LEADING TO UNPREDICTABLE BEHAVIOUR.**

Put the aircraft level in the attitude you expect it to fly in cruise. You need to bear in mind that the control system will make every effort to get the aircraft to this attitude when you let go the sticks.

Power up with UAVPSet connected.  Select the Cal Accs/Gyros test. Confirm you are ready to go by selecting Continue.   You will see the blue LED start to flash - it has already taken the first set of readings and is waiting for the temperature to rise to take the second set.  Get a hair dryer and SLOWLY warm up the board - put your hand close to the board to sense how hot the air is. If it is too hot for your hand it is too hot for the board! You only need to change its temperature by 20 Celsius all up so don't melt the board!  There is a delay as the heat gets into the MPU6050 package so slowly/slowly until the blue LED stops flashing and you are done. Obviously it is better to do the calibration so it covers the range of temperatures you are likely to be flying in.

Now select the Acc/Gyro test to check that the Roll/Pitch/Yaw rate are close to zero. Also check the Acc values which should show Front/Back (FB) and LR close to zero and UD close to -1.

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVPSet_Cal.JPG

Once again you only need to do this once or when loading the defaults or if you change the gyro selection pulldown - it should be on UAVXArm32 unless you are using analog gyros.

The Green LED and Yellow LEDs should be on.

### Step 5 (Magnetometer Calibration) ###

Run the magnetometer calibration test in UAVPSet. Rotate the aircraft in all the directions you can think of including upside down and on edge. This captures the sensor offsets for the X,Y and Z axes using a little tricky math including fitting points to a sphere. You do not need to re-run the calibration unless it is clearly giving crazy answers for the computed compass heading. The offsets are stored in non-volatile memory where they are retained after disconnecting the battery. The magnetometer test should show the Neg/Pos values as being roughly equal. These values show how well you have rocked the aircraft around and correspond to visits to the octants around the axes.


At this point you should run the setup test and see if all of the alarms are cleared. If they are you are almost ready to fly but don't put the prop on yet.

### Step 6 (Servos) ###

Connect your servos up for your aircraft type as shown in the Appendix below.

  * check whether the servo sense/direction is correct for the servos which will now respond to stick input even if disarmed. The motor should not start but leave the prop off anyway.
  * change the sense where required - the sense checkboxes are in the same order as the servo connections
  * increase/decrease the new "roll to yaw" coupling parameter-steering is by roll which is the ailerons which we do not have so you need to couple the ailerons to the rudder.

Put the aircraft on the ground. Leave the prop OFF and switch the arming switch on. UAVPSet communications are terminated once armed - you need to disarm to re-establish communications.

You will see a dancing pattern of Yellow and Blue LEDs while the Black Box memory is cleared; this takes about 8 seconds followed by a single beep. You should hear three starting beeps with the Red LED flashing briefly. At the end you should have Green and Red LEDs on. The Red LED means that no GPS signals are being received.

Now lift the aircraft up and tilt it left/right, forward/backwards. It should be obvious what the control surfaces will do.

**Now some bad news. The Tx sub-trims will not work as, if you think about it, UAVX does not have access to the Rx channels as they are not connected directly to servos. So you need to do, is what in theory we all should do, and that is set all the sub-trims on your Tx to zero and then mechanically adjust the linkages to all of the control surfaces so that the aircraft will fly even with the Tx off.**

You will **definitely** need to adjust the PID parameters until the control surface deflections for your particular aircraft seem reasonable. It is better to have too small a control movement and then increase it if it is not enough.

If everything seems OK you can power down and make your own decision whether it is SAFE to fit the propellor and fly.

**THIS IS NOT A FLIGHT TUTORIAL SO FROM THIS POINT IT IS UP TO YOU.**

Join us at:

http://www.rcgroups.com/forums/showthread.php?t=1093510

## Appendices ##

### Motor/Servo Connector Assignments ###

The "K" output pin assignments which you should test with care are currently:

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVXArm32F4_Outputs.JPG
