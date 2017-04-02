### Loading Firmware ###

Your board will come with the appropriate firmware loaded. To load new firmware you use the STM Flash Loader in the distribution package.

The following sequence of screen shots is self explanatory.

First before connecting power put a link on the **Boot0** pins. This places the Arm processor in bootloader mode when powered up.

Set the baud rate to 115K and the COM port as necessary.


![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM1.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM1.JPG)

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM2.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM2.JPG)

Select the STM32F4-1024K.

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM3.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM3.JPG)

Select the hex file you wish to load. Set the radio button to "necessary pages".

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM4.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM4.JPG)

![https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM5.JPG](https://github.com/gke/UAVXArm/blob/master/wiki/graphics/STM5.JPG)

Wait until the verify is complete. Remove power and then remove the Boot0 link. Verify that the new firmware responds using UAVPSet.
