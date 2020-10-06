# UAVX Failsafe Operation #

Modern receivers have failsafe capability where the channel values go to a known state if the transmitter signal is lost. It is IMPORTANT to set the failsafe values to something sensible e.g. RTH and not pitch/roll/yaw inputs. UAVX will attempt to do what the receiver commands.