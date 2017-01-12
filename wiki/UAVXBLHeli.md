# UAVX BLHeliSuite Interface #

The UAVX firmware can now interface with BLHeliSuite allowing ESCs to be programmed with SimonK or BLHeli firmware. 

You will need to download and install BLHeliSuite initially.

## Procedure ##

REMOVE ALL PROPELLORS.

Connect your multicopter as you would for UAVXGUI but do NOT launch UAVXGUI. Instead launch the BLHeliSuite application.

Power up UAVX should first display a Blue LED accompanied by beeps from the ESCs. This then followed by a Green LED for 5 seconds. During thses 5 seconds it will listen for connect messages from BLHeli 
and so you need to hit the connect button within those 5 seconds. 

Consult the BLHeliSuite documentation (which you should have done already) and proceed as it instructs.

When you are finished hit the disconnect buttion and close BLHeliSuite. UAVX should then complete its initialisation and can be ci=onnected to UAVXGUI if you wish.

If you do not wish to use BLHeliSuite simply wait the additional after powering up and initialisation will continue as usual albeit with the additional short delay.

Remember REMOVE THE PROPELLORS. 