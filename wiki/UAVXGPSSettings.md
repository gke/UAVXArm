# UAVX GPS Settings #

UAVX can accept NMEA or uBlox information which is transmitted as "sentences". The currently preferred GPS unit is a uBlox M8N. 

In the case of uBlox GPS units you can preconfigure them, using u-center free app, or let UAVX configure them using either "M8 Init" or "Legacy Init" in the GUI GPS pulldown.  For most GPS units you only need to use the initialisation once after that you may select just the appropriate unit.
UAVX expects PVT sentences for M8N units and SOL, VELNED and POSSLLH for earlier versions of the uBlox units.

If you have a GPS unit that only transmits the original NMEA sentences then you require GGA and RMC sentences. You will have to initialise the GPS unit manually as UAVX does not do it for you. 

CAUTION: You should only configure the sentences required otherwise UAVX will just have to receve and discard them wasting processing time or simply sending more data than the serial link can support at a minimum of 5 updates per second.