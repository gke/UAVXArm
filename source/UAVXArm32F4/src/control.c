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

#include "UAVX.h"

#define ALT_HOLD_MAX_ROC_MPS 0.2f // Must be changing altitude at less than this for alt. hold to be detected
#define NAV_RTH_LOCKOUT_ANGLE_RAD DegreesToRadians(10)

AxisStruct A[3];
AltStruct Alt;

real32 CameraAngle[3];
real32 OrbitCamAngle = 0.0f;
real32 AngleE, RateE;
int16 AttitudeHoldResetCount;
real32 AttitudeThrFF;

real32 DesiredAltitude, Altitude;
real32 AltComp, ROC, MinROCMPS, EffMinROCMPS;
real32 AltCompDecayS;
real32 TiltThrFFComp = 1.0f;
real32 BattThrFFComp = 1.0f;
real32 AltAccComp = 0.0f;
real32 HorizonTransPoint;
real32 AngleRateMix = 1.0f;
real32 YawStickScaleRadPS, RollPitchStickScaleRadPS;
real32 MaxAttitudeAngleRad;
real32 YawStickThreshold;
real32 DesiredROC = 0.0f;
real32 CruiseThrottle = 0.5f;
real32 CurrMaxTiltAngle = 0.0f;
real32 TuneKpScale;
int8 BeepTick = 0;

real32 FWRollPitchFFFrac, FWAileronDifferentialFrac, FWPitchThrottleFFFrac,
		MaxAltHoldCompFrac, MaxAttitudeAngleRad, FWMaxClimbAngleRad,
		FWGlideAngleOffsetRad, FWFlapDecayS, BestROCMPS;

void ZeroCompensation(void) {
	AltComp = 0.0f;
	BattThrFFComp = TiltThrFFComp = 1.0f;
} // ZeroCompensation


void CalcTiltThrFFComp(void) {
	const real32 TiltFFLimit = 1.0f / cosf(NAV_MAX_ANGLE_RAD);
	real32 Temp;

	if (IsMulticopter && (State == InFlight) && !F.UsingRateControl) { // forget near level check
		Temp = (1.0f / AttitudeCosine() - 1.0) * AttitudeThrFF + 1.0f;
		Temp = Limit(Temp, 1.0f, TiltFFLimit);
		TiltThrFFComp = SlewLimit(TiltThrFFComp, Temp, TiltFFLimit, dT);
	} else
		TiltThrFFComp = 1.0f;

} // CalcTiltThrFFComp


void CalcBattThrComp(void) {

	BattThrFFComp
			= IsMulticopter && (State == InFlight) && !F.UsingRateControl ? BatterySagR
					: 1.0f;

} // CalcBattThrComp

//______________________________________________________________________________


void TrackCruiseThrottle(void) {

	if (F.Emulation)
		CruiseThrottle = IsFixedWing ? THR_DEFAULT_CRUISE_FW
				: THR_DEFAULT_CRUISE;
	else {
		//	if (IsFixedWing)
		//		CruiseThrottle = THR_DEFAULT_CRUISE_FW;
		//	else {
		if ((Abs(ROC) < ALT_HOLD_MAX_ROC_MPS) && (DesiredThrottle
				> THR_MIN_CRUISE)) {
			CruiseThrottle += (DesiredThrottle > CruiseThrottle ? 0.002f
					: -0.002f) * AltdT;
			CruiseThrottle
					= Limit(CruiseThrottle, THR_MIN_CRUISE, THR_MAX_CRUISE );
			SetP(CruiseThr, (uint8) (CruiseThrottle * 100.0f));
		}
		//	}
	}
} // TrackCruiseThrottle


void DoROCControl(real32 DesiredROC, real32 MinROCMPS, real32 MaxROCMPS) {
	real32 Pr, Dr, ROCE;

	ROCE = Limit(DesiredROC, MinROCMPS, MaxROCMPS) - ROC;

	Pr = ROCE * Alt.I.Kp;
	Dr = AltAccComp = Limit1(AccZ * Alt.I.Kd, 0.2f);

	AltComp = Limit1(Pr + Dr, IsFixedWing ? 0.5f : MaxAltHoldCompFrac);

} // DoROCControl


void AltitudeHold(real32 AltE, real32 MinROCMPS, real32 MaxROCMPS) {
	real32 Pa, Ia;

	Pa = AltE * Alt.O.Kp;

	Alt.O.IntE += AltE * Alt.O.Ki * AltdT;
	Alt.O.IntE = Limit1(Alt.O.IntE, Alt.O.IL);
	Ia = Alt.O.IntE;

	DesiredROC = Pa + Ia;

	DoROCControl(DesiredROC, MinROCMPS, MaxROCMPS);

} // AltitudeHold

//______________________________________________________________________________

// Fixed Wing

void DeploySpoilers(real32 a) {

	Sl = Limit(a * 0.05f, 0.0, 1.0f);
	if (Sl > 0.0f)
		AltComp = -1.0f;

} // DeploySpoilers

void AcquireAltitudeFW(void) {

	real32 AltE = DesiredAltitude - Altitude; // negative then too high => descend

	CheckRapidDescentHazard();
	if (F.RapidDescentHazard)
		DeploySpoilers((-AltE) - DESCENT_ALT_DIFF_M);
	else {
		AltitudeHold(AltE, -BestROCMPS, BestROCMPS);
		Sl = DecayX(Sl, FWFlapDecayS, AltdT);
	}

} // AcquireAltitudeFW

boolean ROCTooHigh(real32 Window) {

	return UsingDCMotors ? false : (Abs(ROCF) > Window);

} // ROCTooHigh

void AltitudeControlFW(void) {

	if (F.Glide && F.NavigationEnabled) {
		if (NavState == BoostClimb) {
			F.HoldingAlt = true;
			Sl = 0.0f;
			DoROCControl(BestROCMPS, 0.0, BestROCMPS);
		} else {
			//Sl = (NavState == AltitudeLimiting) ? Limit(Abs(Altitude - AltMaxM) * 0.05f, 0.0f, 1.0f)
			//				: 0.0f; // TODO: slew
			DeploySpoilers(Abs(Altitude - AltMaxM));
			F.HoldingAlt = false;
			AltComp = -1.0f;
		}
	} else {
		if (F.SticksUnchangedFailsafe || F.BatteryFailsafe || ((NavState
				!= HoldingStation) && (NavState != PIC))) { // Navigating - using CruiseThrottle
			F.HoldingAlt = true;
			AcquireAltitudeFW();
		} else {
			CheckThrottleMoved();
			if (F.ThrottleMoving || (ROCTooHigh(1.0f) && !F.HoldingAlt)) {
				F.HoldingAlt = false;
				DesiredAltitude = Altitude;
				Alt.O.IntE = Sl = 0.0f;
				AltComp = DecayX(AltComp, AltCompDecayS, AltdT);
			} else {
				F.HoldingAlt = true;
				TrackCruiseThrottle();
				AcquireAltitudeFW(); // using Stick Throttle NOT cruise throttle
			}
		}
	}
} // AltitudeControlFW

//______________________________________________________________________________


void AcquireAltitude(void) {
	real32 AltE;
	// Synchronised to baro intervals independent of active altitude source
	real32 EffMinROCMPS;

	EffMinROCMPS = (F.RapidDescentHazard || (NavState == ReturningHome)
			|| (NavState == Transiting)) ? DESCENT_MIN_ROC_MPS : MinROCMPS;

	AltE = DesiredAltitude - Altitude;

	AltitudeHold(AltE, EffMinROCMPS, ALT_MAX_ROC_MPS);

} // AcquireAltitude


void AltitudeControl(void) {

	if (F.SticksUnchangedFailsafe || F.BatteryFailsafe || ((NavState
			!= HoldingStation) && (NavState != PIC))) { // Navigating - using CruiseThrottle
		F.HoldingAlt = true;
		AcquireAltitude();
	} else {
		CheckThrottleMoved();
		if (F.ThrottleMoving || (ROCTooHigh(0.25f) && !F.HoldingAlt)) {
			F.HoldingAlt = false;
			DesiredAltitude = Altitude;
			Alt.O.IntE = 0.0f;
			AltComp = DecayX(AltComp, AltCompDecayS, AltdT);
		} else {
			F.HoldingAlt = true;
			TrackCruiseThrottle();
			AcquireAltitude(); // using Stick Throttle NOT cruise throttle
		}
	}
} // AltitudeControl


void DoAltitudeControl(void) {

	if (F.NewAltitudeValue) { // every AltdT
		F.NewAltitudeValue = false;

		if (IsFixedWing)
			UpdateVario();

		if (F.AltControlEnabled && !F.Bypass) {
			if (IsFixedWing)
				AltitudeControlFW();
			else
				AltitudeControl();
		} else {
			//zzz check
			F.RapidDescentHazard = ROC < DESCENT_MIN_ROC_MPS;
			DesiredAltitude = Altitude; // zzz redundant
			AltComp = DecayX(AltComp, AltCompDecayS, AltdT);
			Sl = DecayX(Sl, FWFlapDecayS, AltdT);
			F.HoldingAlt = false;
		}
	}

} // DoAltitudeControl


void DetermineControl(void) {

	A[Pitch].Control = Threshold(A[Pitch].Desired, THRESHOLD_STICK)
			* MaxAttitudeAngleRad;
	A[Roll].Control = Threshold(A[Roll].Desired, THRESHOLD_STICK)
			* MaxAttitudeAngleRad;
	A[Yaw].Control = Threshold(A[Yaw].Desired, YawStickThreshold)
			* YawStickScaleRadPS;

	F.YawActive = IsFixedWing ? Max(Abs(A[Yaw].Control), Abs(A[Roll].Control))
			> 0.0f : Abs(A[Yaw].Control) > 0.0f;

	if (F.NavigationEnabled) {
		A[Pitch].Control += A[Pitch].NavCorr;
		A[Roll].Control += A[Roll].NavCorr;
		// not used A[Yaw].Control += A[Yaw].NavCorr;
	}

} // DetermineControl


real32 ComputeRateDerivative(AxisStruct *C, real32 RateE) {
	real32 r;

	r = LPFilter(&C->RateF, RateE, PROP_LP_FREQ_HZ, dT);
	C->RateD = (r - C->Ratep) * dTR;
	C->Ratep = r;
	C->RateD = Smoothr32xn(&C->RateDF, 4, C->RateD);

	return (C->RateD);

} // ComputeRateDerivative


void ZeroPIDIntegrals(void) {
	int32 a;

	for (a = Pitch; a <= Yaw; a++)
		A[a].O.IntE = A[a].I.IntE = 0.0f; // TODO: I.IntE unused

} // ZeroPIDIntegrals


void DoRateControl(int32 a) {
	real32 DesiredAngle, AngleE, DesiredRate, RateE;
	real32 AngleRateMix;
	real32 Pa, Pr, Dr;
	AxisStruct *C;

	C = &A[a];

	if (P(Horizon) > 0) { // hybrid that panics back to angle when sticks centred

		AngleRateMix
				= Limit(1.0f - (CurrMaxRollPitchStick / HorizonTransPoint), 0.0f, 1.0f);

		DesiredAngle = Limit1(C->Control * AngleRateMix, C->O.Max);

		AngleE = DesiredAngle - C->Angle;

		Pa = AngleE * C->O.Kp;
		C->O.IntE = 0.0f; // for flip back to angle mode

		DesiredRate = Pa + C->Control * (RollPitchStickScaleRadPS
				/ MaxAttitudeAngleRad) * (1.0f - AngleRateMix);
	} else
		// pure rate control
		DesiredRate = C->Control * RollPitchStickScaleRadPS
				/ MaxAttitudeAngleRad;

	RateE = Rate[a] - Limit1(DesiredRate, C->I.Max);

	Pr = RateE * C->I.Kp;
	Dr = ComputeRateDerivative(C, RateE) * C->I.Kd;

	C->Out = Limit1(Pr + Dr, 1.0f);

} // DoRateControl


void DoAngleControl(int32 a) { // with Ming Liu
	real32 DesiredAngle, AngleE, DesiredRate, RateE;
	real32 Pa, Ia, Pr, Dr;
	AxisStruct *C;

	C = &A[a];

	DesiredAngle = Limit1(C->Control, C->O.Max);

	AngleE = DesiredAngle - C->Angle;

	Pa = AngleE * C->O.Kp;

	C->O.IntE += AngleE * C->O.Ki * dT;
	C->O.IntE = Limit1(C->O.IntE, C->O.IL);
	Ia = C->O.IntE;

	DesiredRate = Limit1(Pa + Ia, C->I.Max);

	RateE = Rate[a] - DesiredRate;

	Pr = RateE * C->I.Kp;

	Dr = ComputeRateDerivative(C, RateE) * C->I.Kd;

	C->Out = Limit1(Pr + Dr, 1.0f);

} // DoAngleControl


real32 MinimumTurn(real32 Desired) {
	real32 HeadingE, absHeadingE;
	static real32 TurnSign;
	static boolean TurnCommit = false;

	HeadingE = MakePi(Desired - Heading);

	if (IsFixedWing) {
		if (NavState == UsingThermal) {
			TurnCommit = true;
			TurnSign = 1.0f;
			HeadingE = Make2Pi(Desired - Heading); // turn right
		} else {
			HeadingE = MakePi(Desired - Heading);
			absHeadingE = fabsf(HeadingE);
			if (absHeadingE > DegreesToRadians(160)) {
				TurnCommit = true;
				TurnSign = Sign(HeadingE);
			} else if (absHeadingE < DegreesToRadians(135))
				TurnCommit = false;

			if (TurnCommit)
				HeadingE = TurnSign * absHeadingE;
		}
	}
	return (HeadingE);

} // MinimumTurn

static void DoYawControlFW(void) {
	real32 Pa, Pr, RateE;
	static real32 DesiredRate = 0.0f;
	static real32 KpScale = 1.0f;

	if (F.YawActive) {
		DesiredHeading = Heading;
		KpScale = 1.0f;
	} else if (NavState != PIC) {
		DesiredHeading = Nav.DesiredHeading;
		KpScale = Nav.Sensitivity;
	}

	HeadingE = MinimumTurn(DesiredHeading);
	HeadingE = Limit1(HeadingE, DegreesToRadians(150)); // 30

	Pa = HeadingE * A[Yaw].O.Kp * 0.05f * KpScale;

	DesiredRate = Pa;

	RateE = (DesiredRate + A[Yaw].Control) - Rate[Yaw];
	Pr = RateE * A[Yaw].I.Kp;

	A[Yaw].Out = Limit1(Pr, 1.0);

	real32 Temp = A[Roll].NavCorr;
	//A[Roll].NavCorr = 2.0f * Pr * NAV_MAX_ANGLE_RAD;
	A[Roll].NavCorr = atanf(Limit1(DesiredRate * Airspeed *
			GRAVITY_MPS_S_R, NAV_MAX_ANGLE_RAD));
	A[Roll].NavCorr = SlewLimit(Temp, A[Roll].NavCorr, DegreesToRadians(10.0f),
			dT);

} // DoYawControlFW


static void DoYawControl(void) {
	real32 Pa, Pr, RateE;
	static real32 DesiredRate = 0.0f;

	if (F.UsingRateControl) {

		DesiredRate = Limit1(A[Yaw].Control, A[Yaw].I.Max);
		RateE = DesiredRate - Rate[Yaw];
		Pr = RateE * A[Yaw].I.Kp;

		A[Yaw].Out = Limit1(Pr, 1.0);

	} else {

		if (F.YawActive || F.OrbitingWP || F.RapidDescentHazard || F.UsingPOI)
			DesiredHeading = F.YawActive ? Heading : Nav.DesiredHeading;

		HeadingE = MinimumTurn(DesiredHeading);
		HeadingE = Limit1(HeadingE, DegreesToRadians(30));

		Pa = HeadingE * A[Yaw].O.Kp;

		DesiredRate = Limit1(Pa, A[Yaw].I.Max);

		RateE = (DesiredRate + A[Yaw].Control) - Rate[Yaw];
		Pr = RateE * A[Yaw].I.Kp;

		A[Yaw].Out = Limit1(Pr, 1.0f);
	}

} // DoYawControl


void DoControl(void) {
	int32 a;

	CurrMaxTiltAngle = Max(Abs(A[Roll].Angle), Abs(A[Pitch].Angle));
	F.NearLevel = CurrMaxTiltAngle < NAV_RTH_LOCKOUT_ANGLE_RAD;

	//CalcTiltThrFFComp();
	//CalcBattThrComp();

	DetermineControl();

	if (IsFixedWing)
		DoYawControlFW(); // must do first for fixed wing turn coordination
	else
		DoYawControl();

	for (a = Pitch; a <= Roll; a++)
		if (F.UsingRateControl)
			DoRateControl(a);
		else
			DoAngleControl(a);

	UpdateDrives();

} // DoControl


void InitControl(void) {
	int32 a;
	AxisStruct *C;

	HeadingE = Sl = 0.0f;
	ZeroCompensation();

	for (a = Pitch; a <= Yaw; a++) {
		C = &A[a];
		C->O.IntE = 0.0f;
		C->O.Dp = 0.0f;
		C->Ratep = 0.0f;
		C->I.Dp = 0.0f;

		C->NavCorr = 0.0f;
		C->RateF.Primed = false;
		C->RateDF.Primed = false;

		C->Out = 0.0f;
	}

	Acc[Z] = -GRAVITY_MPS_S;

} // InitControl


