# UAVX BLHeliSuite Interface #

The UAVX firmware can now interface with BLHeliSuite allowing ESCs to be programmed with SimonK or BLHeli firmware. 

You will need to download and install BLHeliSuite initially.

https://blhelisuite.wordpress.com/

## Procedure ##

REMOVE ALL PROPELLORS.

Consider using a current limited power supply if you have one instead of a battery.

Connect your multicopter as you would for UAVXGUI but do NOT launch UAVXGUI. Instead launch the BLHeliSuite application.

Power up UAVX which should first display a BLUE LED accompanied by beeps from the ESCs. This is then followed by a YELLOW LED for 5 seconds. During these 5 seconds it will listen for connect messages from BLHeli 
and so you need to hit the connect button within those 5 seconds. 

Consult the BLHeliSuite documentation (which you will have done already) and proceed as it instructs.

If you are not programming ESCs and just using UAVXGUI do not connect until after the YELOW LED is off otherwise UAVX will think UAVXGUI is BLHeliSuite and hang!

https://blhelisuite.wordpress.com/

When you are finished hit the disconnect button and close BLHeliSuite. UAVX should then complete its initialisation and can be connected to UAVXGUI if you wish.

If you do not wish to use BLHeliSuite simply wait the few extra seconds after powering up and initialisation will continue as usual.

Remember REMOVE THE PROPELLORS. 
