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


#ifndef _inertial_h
#define _inertial_h

extern uint8 CurrStateEst;

enum StateEstimators {
	MadgwickIMU, MadgwickAHRS, MadgwickMARG, EstUnknown
};

void InitMadgwick(void);
void UpdateInertial(void);

void GetIMU(void);

void CheckNavEnable(void);

void ShowIMUType(uint8 s);

real32 GravityCompensatedAccZ(void);
real32 AttitudeCosine(void);

extern const char * IMUName[];

extern real32 AccConfidenceSDevR, AccConfidence;
extern real32 KpAccBase, KpMagBase, BetaBase;

extern HistStruct AccZF;
extern real32 AccZ;
extern real32 AltLPFHz;

extern uint32 LastInertialUpdateuS;

#endif

