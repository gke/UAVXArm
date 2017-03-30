// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU
//    General Public License as published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.
//    If not, see http://www.gnu.org/licenses/


#ifndef _tune_h
#define _tune_h

// NoTuning, RollAngleP, RollAngleI, RollRateP, RollRateD, PitchAngleP,
// PitchAngleI, PitchRateP, PitchlRateD, YawAngleP, YawRateP,
// AltitudeErrorP, AltitudeErrorI, AltitudeROCP, AltitudeROCD,
// NavPositionP, NavVelP, NavVelI, NavCrossTrack, UnknownParameter

extern real32 TuningScale;
extern uint8 CurrTuningSel;
extern boolean Tuning;
extern uint8 OldUntunedParam;
extern uint8 TuningParamIndex;

void Tune(void);
void InitTune(void);

#endif



