UAVXArm32F4
===========

UNDER CONSTRUCTION

An initial UAVXArm prototype first flew in November 2010 but did not proceed. It was decided in 2012 to restart the development and proceed with a release the progress details of which can be found in the Wiki:  

[http://code.google.com/p/uavp-mods/wiki/UAVXArm]

These pages are now directed at developments centred around UAVXArm but will continue to provide support and transition paths for those currently committed to UAVP.

=== History ===

This Project was originally a collection of modifications, experiments, hardware improvements and alternate flight and test software for multicopters focussed around one of the very first quadrocopters - UAVP.

UAVX (C) was a new version of UAVP flight control software. It remains free software released under the terms of the *GNU General Public License*. UAVX used the "Historic/Legacy" UAVP *Black Board* famed for its simplicity without sacrificing performance.

===UAVXArm Functionality===

UAVXArm has the following functionality:

  * Full autonomous navigation with 3D (altitude and position) acquisition, auto-takeoff and landing. 4D functionality (time) may be activated later.

  * Failsafe and selectable return to home with selectable auto-land and motor shutdown.

  * Ultrasonic rangefinder support for precision low-hover and landing.

  * Quadrocopter, Tricopter, Octocopter, Hexacopter, Helicopter and some fixed wing aircraft. 

  * PWM and I2C ESCs supported. 

  * Support for a wide range of Tx/Rx combinations and custom configuration capability for new combinations.
 
  * Flight simulation mode for mission planning and verification.

  * Comprehensive flight telemetry including raw GPS, byte stuffed binary, and text compatible hand-held ground-stations including those developed by contributors to this thread.

UAVXArm is supported by:

  * *UAVXNav*: A mission planner with integrated ground-station for use in flight. 
  * *UAVXGS*: A ground-station which displays a much larger set of internal UAVX variables including sensor values whilst in flight. 
  * *UAVPSet*: For setting UAVX control parameters and sensor testing.

The relevant discussion threads for the project are:

  * http://www.rcgroups.com/forums/showthread.php?t=811550 (UAVP Hardware )
  * http://www.rcgroups.com/forums/showthread.php?t=1093510 (UAVX Software)

For those interested in quadrocopter propulsion we recommend:

  * http://www.rcgroups.com/forums/showthread.php?t=1006721 (Propulsion)

See the *Wiki* tab for current documentation and ideas currently being considered for the project.

===Copyright===

The software on this site is NOT FOR COMMERCIAL USE OR PROFIT - See the GNU General Public License.

===Acknowledgement===

We wish to acknowledge the original UAVP contributions of Dr. Eng. Wolfgang Mahringer.

===Disclaimer===

This project is not endorsed by nor is it part of the official UAVP project. Our intent here is not to detract from the excellent work that the UAVP creators have done, but to provide a playground for project members to experiment with different ideas. 
