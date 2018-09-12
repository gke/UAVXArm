IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 

The output numbering for iNav is different to UAVX. 

This is important for FIXED WING aircraft as UAVX uses pin K1 for 
the motor whereas iNav uses UAVXF4 pins K5 and K6. Failure
to pay attention to this can result in a motor being connected 
to K1 which is biased to 1.5mS the midpoint for a servo 
but 50% throttle!

UAVX is	iNav
K1      3
K2      4
K3      5
K4      6
K5      1 Motor
K6      2 Motor
K7      not used
K8      not used
K9      not used
K10     not used