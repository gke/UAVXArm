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


#ifndef _autonomous_h
#define _autonomous_h

enum Coords {
	NorthC, EastC, DownC
};

typedef struct {
	real32 Desired[3];
	real32 Acc[3];
	real32 Pos[3], PosE[3], PosEP[3];
	real32 DesVel[3], Vel[3], VelP[3], VelE[3], VelEP[3], VelIntE[3];
	real32 Corr[3], CorrP[3];

	real32 Sensitivity, KpEff, KiEff;
	real32 TakeoffBearing;
	real32 Distance, Bearing, Elevation;
	real32 Hint;

	real32 DesiredVel, DesiredHeading;

	real32 WPDistance, OriginalWPBearing, WPBearing;
	real32 CrossTrackKp, CrossTrackE;
	real32 CrossTrackPosE[2];
	real32 FenceRadius;

	real32 LPFCutOffHz;
	PIDStruct O, I;

	real32 MaxVelocity, VelocityLimit;
	real32 MaxAngle;
	real32 YawKp;
} NavStruct;

extern NavStruct Nav;

enum NavStates {
	HoldingStation,
	ReturningHome,
	AtHome,
	Descending,
	Touchdown,
	Transiting,
	Loitering,
	OrbitingPOI,
	Perching,
	Takeoff,
	PIC,
	AcquiringAltitude,
	UsingThermal,
	UsingRidge,
	UsingWave,
	BoostClimb,
	AltitudeLimiting,
	JustGliding,
	NavStateUndefined
};

enum NavComs {
	navVia, navOrbit, navPerch, navPOI, navGlide, navUnknown
};

extern const char * NavComNames[];

typedef struct {
	real32 Pos[3];
	real32 Velocity;
	int16 Loiter;
	real32 OrbitRadius;
	real32 OrbitAltitude;
	real32 OrbitVelocity;
	uint8 Action;
} WPStruct;

enum FailStates {
	NoFailsafes, Monitoring, BatteryLow, LostSignal, HitFenceRTH
};

enum LandingStates {
	InitDescent, CommenceDescent, Descent, DescentStopped
};

enum NavSwStates {
	NavSwLow, NavSwMiddle, NavSwHigh, NavSwUnknown
};

extern void DoNavigation(void);
extern void InitNavigation(void);

extern void UpdateRTHSwState(void);
extern void CheckProximity(void);
extern void ResumeHoldingStation(void);
extern void AcquireHoldPosition(void);
extern void CheckRapidDescentHazard(void);

extern void DecayPosCorr(void);
extern void DoShutdown(void);

extern uint8 NavState, NavSwState, NavSwStateP, FailState;
extern real32 DesiredNavHeading;

#endif


