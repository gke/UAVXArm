## Introduction ##

This is the evolving setup page for the Arm version of UAVX. We will try to keep explanations concise. If you have flown UAVX then most of this is familiar as we have deliberately scaled the parameters and kept the look and feel of UAVPSet and UAVXGS to those you are familiar with so you can concentrate on flying.

UAVXStartup contains a wealth of information which you could take the time to read. You should pay particular attention to what the different patterns of LED behaviour mean (Lights and Sirens).

UAVXArm32F4 supports many different airframe types from octocopters, through helis out to conventional aircraft. UAVX has done this for several years. Most of the multicopter configurations are shown in UAVXStartup. If you are flying a fixed wing aircraft you should read the UAVXArm32F4FixedWingStartup Wiki as well.

If you are using UAVXArm32F4 board without sensors as an adapter for the old UAVP board you should read the associated Wiki. If you are doing this then you will be quite familiar with UAVX/UAVP but it is still worth reading the notes below.

#### UAVXArm32F4 Board ####

![https://github.com/gke/uavp-mods/blob/uavx_graphics/Uavx-nanoV4.png](https://github.com/gke/uavp-mods/blob/uavx_graphics/Uavx-nanoV4.png)

You should receive your board loaded with firmware compatible with UAVXGUI. You definitely should look at the UAVXGUI before you go any further. You will be bale to load firmware updates later - see the UAVXArm32F4LoadingFirmware Wiki.

### Step 1 ###

Mount the board on your favourite frame oriented in the direction of forward flight. So if you are flying +Mode point the board along the K1 motor arm. If you are flying XMode point the board between the K1 and K3 motor arms. Select the desired configuration in UAVPSet using the airframe pulldown.

Take all the the shorting links (if any) off the board. Connect up an arming switch between the Arming pin and the adjacent Ground pin.

The motors should not run even if you have the arming switch on until you get through all of the following steps but think about a Kw of motors all running at around 7000RPM flying around your workshop - take the props OFF.

The board can be connected directly to your main battery (3S LiPo).

The Red light should be on and the Yellow LED should be flashing once a second indicating that you have not yet calibrated your accelerometers.

### Step 2 ###

Download the software package from the downloads area and install UAVXGUI.

Connect the adapter lead to the board and then to a USB port on your computer.

Start UAVXGUI and check that you have the appropriate COM port selected and that the baud rate is set to 115Kbaud.

Select the parameters button and read the current parameters which will be the defaults.

The default parameters were obtained from flights by Ken & Jim. These are a good starting point but you will probably wish to tune the Roll/Pitch and Yaw parameters in particular to suit your own flying style and the size/span and weight of your aircraft.

### Step 3 (Rx) ###

Important - the black or ground pins of your Rx and Motor connections should be connected to the pins closest to the edge of the board. If in doubt double check with a multimeter.

From now on any changes you make to parameters in UAVXGUI must be followed by selecting the **Write Config** icon. Some parameter changes involve an electrical reconfiguration of the board so you will occasionally see all LEDs flashing - cycle the power and this should allow the board to complete the reconfiguration.

Connect the front motor control lead to M1. This will power your Rx as centre pins of all of the M and Rx pins are connected together.

If you have a a Rx (e.g. FrSky) that supports Compound PPM (CPPM) connect it to Rx1. For CPPM you must specify the number of channels your Rx is receiving using UAVXGUI.

If you have an Rx that uses parallel PPM then connect it to Rx1 up to Rx8 in the order Throttle, Aileron, Elevator, Rudder, Gear, Aux1, Aux2, Aux3. You need a minimum of 7 channels for full functionality.

For parallel PPM using UAVXGUI set number of channels you have actually connected.

You can fly with a minimum of 4 channels Throttle, Aileron, Elevator, Rudder. If you do so then you will have altitude hold but no navigation capability.

You may re-assign the channels using the selectors in UAVXGUI. So if your throttle is on Channel 3 for example then setup the pulldowns appropriately.

Go to the main UAVXGUI page and turn on your Tx. Stir the Tx sticks and adjust the endpoints and neutral values of those channels that turn orange.

As well as adjusting the navigation sensitivity NavS or Channel 7 (normally a potentiometer/knob) is used to enable altitude hold.  Initially you should fly with altitude hold off. Altitude hold is switched OFF below 1.1mS and ON above 1.1mS. The knob does not control altitude hold sensitivity.

In writing this an auto configure for the Rx has moved to the top of the development list but not for the first release!

The Green LED should be on and Yellow LED flashing every second.

### Step 4 (Acc/Gyro Temperature Calibration) ###

This is the most critical step in the setup and you usually only need to do it once or until you next bend the aircraft.

**YOU MUST CALIBRATE ACCELEROMETERS OTHERWISE THE ATTITUDE ESTIMATES (PITCH/ROLL ANGLES) WILL BE INCORRECT LEADING TO UNPREDICTABLE BEHAVIOUR.**

Put the aircraft on a level surface. If it is on the slightest of angles then this is the way it will fly when you centralise the sticks. Bear in mind the floors of your house will not be level, your best table will not be level nor will your average workbench. Eyeballing it is no good at all.

Get a decent spirit level and even then turn it around to make sure it reads level both ways.

Lock the quadrocopter down so it does not move and recheck it is level.

Power up with UAVGUI connected.  Select the CalIMU button which will be Red. You will see the blue LED start to flash - it has already taken the first set of readings and is waiting for the temperature to rise to take the second set.  Get a hair dryer and SLOWLY warm up the board - put your hand close to the board to sense how hot the air is. If it is too hot for your hand it is too hot for the board! You only need to change its temperature by 20 Celsius all up so don't melt the board!  There is a delay as the heat gets into the MPU6xxx package so slowly/slowly until the blue LED stops flashing and you are done. The CalIMU button should go Green. Obviously it is better to do the calibration so it covers the range of temperatures you are likely to be flying in.

Once again you only need to do this once or when loading the defaults or if you change the gyro selection pulldown - it should be on UAVXArm32 unless you are using analog gyros.

The Green LED and Yellow LEDs should be on.

_Note: See Accelerometer Neutral Fine Tuning below._

### Step 5 (Magnetometer Calibration) ###

Select the CalMag button and rotate the aircraft in all the directions you can think of including upside down and on edge. This captures the sensor offsets for the X,Y and Z axes using a little tricky math including fitting points to a sphere. You do not need to re-run the calibration unless it is clearly giving crazy answers for the computed compass heading. The offsets are stored in non-volatile memory where they are retained after disconnecting the battery. The magnetometer test should show the Neg/Pos values as being roughly equal. These values show how well you have rocked the aircraft around and correspond to visits to the octants around the axes.

If they are you are almost ready to fly but don't put the props on yet.

### Step 6 (Motors) ###

Disconnect the centre leads of the rest of your ESC control leads and make sure they are taped back and insulated.

Connect the other motors:

  * Left K2
  * Right K3
  * Back K4

Note: If you plan to run camera gimbals then it is worth considering buying a high current UBEC and connecting that to an unused Rx or M connector. In that case disconnect all of the BEC centre leads and connect the UBEC to an unused M/Rx connector.

Put the aircraft on the ground. Leave the props OFF and switch the arming switch on.

You will see a dancing pattern of Yellow and Blue LEDs while the Black Box memory is cleared; this takes several seconds followed by a single beep. You should hear three starting beeps with the Red LED flashing briefly. At the end you should have Green and Red LEDs on. The Red LED means that no GPS signals are being received.

Advance the throttle and all motors should start. Run the motors more very slowly as you can damage them if you run at high speed without props. Listening carefully move your controls and verify that if you move the aileron right that the left motor increases speed and the right motor slows down. The same for elevator. Rudder left should slow down the front/back motors and speed up the left/right motors by the same amount.

Now lift the quadrocopter up and tilt it left/right, forward/backwards. It should be obvious what the motors will do. Tipping right should cause the right motor to speed up. Again use very little throttle.

If everything seems OK you can power down and make your own decision whether it is SAFE to fit the propellors and fly.

**THIS IS NOT A FLIGHT TUTORIAL SO FROM THIS POINT IT IS UP TO YOU.**

Join us at:

http://www.rcgroups.com/forums/showthread.php?t=1093510

## Next Steps ##

### GPS Connection and Initialisation ###

The GPS is connected to the I2C/Rangefinder connector. The connector is configured for serial communication at 115KBaud. The GPS Rx line should be connected to SDA and the Tx to CLK. If you intend connecting an I2C capable GPS then we will try to get it done.

In what follows you may wish to set your GPS unit's parameters using the manufacturer's tools. In this case leave the GPS Rx pin disconnected so that UAVX does not use its default configuration.

If using the NMEA protocol you should choose the $GPGGA and $GPRMC sentences at a minimum update rate of 5Hz.

You must have a solid Green LED and a Blue LED flashing at the GPS rate which is normally 5Hz to fly. If you have a Red LED flashing intermittently **do not fly using GPS navigation** as you have poor GPS reception or there is some other fault.

### Accelerometer Neutral Fine Tuning ###

It is inevitable that the accelerometer neutrals will require small adjustments regardless of how well you leveled the aircraft. You can fine tune the neutrals using the Tx sticks.  The procedure is simple and is done when the aircraft is armed:

  * set you Tx aileron and elevator trims to centre (no trim).
  * fly the aircraft pointing away from you and note the directions that it drifts.
  * land and, with the throttle closed but still armed, hold the aileron or elevator stick at maximum in direction opposite to the drift until you hear a beep - there is a delay of 2 seconds to the first beep.
  * Fly again repeating until there is no drift.

You should do this when there is no wind and you only need to do it once. It is always better to adjust the neutrals rather than use the Tx trims if you want the flight controller to work properly.

You should never need yaw trim.

## Appendices ##

### Motor/Servo Connector Assignments ###

The "K" output pin assignments which you should test with care are currently:

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVXArm32F4_Outputs.JPG

### Pin Allocations ###

These are the Arm processor pin assignments.

https://github.com/gke/uavp-mods/blob/uavx_graphics/UAVXArm32F4_V4Pinouts.JPG
