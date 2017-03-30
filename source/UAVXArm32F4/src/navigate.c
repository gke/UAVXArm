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

// Autonomous flight routines

#include "UAVX.h"

real32 NavdT, NavdTR;
uint32 LastNavUpdateuS = 0;
NavStruct Nav;
real32 DesiredVel;
real32 POIHeading = 0.0f;
real32 NorthP, EastP;
uint8 PrevWPNo;
real32 PosXTrack, VelXTrack, PosYTrack, VelYTrack;
boolean ResetNavHold = true;

void RotateWPPath(real32 * nx, real32 * ny, real32 x, real32 y) {
	static real32 wpS = 0.0f;
	static real32 wpC = 1.0f;
	real32 HR, NorthDiff, EastDiff;

	if (CurrWPNo != PrevWPNo) {
		NorthDiff = WP.Pos[NorthC] - NorthP;
		EastDiff = WP.Pos[EastC] - EastP;

		NorthP = WP.Pos[NorthC];
		EastP = WP.Pos[EastC];

		HR = sqrtf(Sqr(EastDiff) + Sqr(NorthDiff));
		if (HR > 0.1f) {
			HR = 1.0f / HR;
			wpS = EastDiff * HR;
			wpC = NorthDiff * HR;
		}
		PrevWPNo = CurrWPNo;
		F.CrossTrackActive = true;
	}

	*nx = x * wpC + y * wpS;
	*ny = -x * wpS + y * wpC;

} // RotateWPPath


real32 Aerosonde(void) {
	// "Lateral Track Control Law for Aerosonde UAV", M. Niculescu,
	// Paper 16, AIAA, 8-11 January 2001, Reno, NV
	real32 DesYawRate;

	RotateWPPath(&PosXTrack, &PosYTrack, Nav.PosE[NorthC], Nav.PosE[EastC]);
	RotateWPPath(&VelXTrack, &VelYTrack, Nav.Vel[NorthC], Nav.Vel[EastC]);

	DesYawRate = (0.2f * PosXTrack * VelYTrack - PosYTrack * VelXTrack)
			* 0.0025;

	return (Limit1(DesYawRate, NAV_YAW_MAX_SLEW_RAD_S));

} // Aerosonde

//_______________________________________________________________________________

//Bank angle command based on angle between aircraft velocity vector and reference vector to path.
//S. Park, J. Deyst, and J. P. How, "A New Nonlinear Guidance Logic for Trajectory Tracking,"
//Proceedings of the AIAA Guidance, Navigation and Control
//Conference, Aug 2004. AIAA-2004-4900.
//Modified to use PD control for circle tracking to enable loiter radius less than L1 length
//Modified to enable period and damping of guidance loop to be set explicitly
//Modified to provide explicit control over capture angle

// Period in seconds of L1 tracking loop. This parameter is the primary control for agressiveness of turns in auto mode. This needs to be larger for less responsive airframes. The default of 20 is quite conservative, but for most RC aircraft will lead to reasonable flight. For smaller more agile aircraft a value closer to 15 is appropriate, or even as low as 10 for some very agile aircraft. When tuning, change this value in small increments, as a value that is much too small (say 5 or 10 below the right value) can lead to very radical turns, and a risk of stalling.
real32 L1Period = 20.0f;
// Damping ratio for L1 control. Increase this in increments of 0.05 if you are getting overshoot in path tracking. You should not need a value below 0.7 or above 0.85.
real32 L1Damping = 0.75f;
//Crosstrack error integrator gain. This gain is applied to the crosstrack error to ensure it converges to zero. Set to zero to disable. Smaller values converge slower, higher values will cause crosstrack error oscillation.
// @Range: 0 0.1 @Increment: 0.01
real32 _L1_ForwardTrack_i_gain = 0.02f;

real32 _L1_ForwardTrack_i, _L1_ForwardTrack_i_gain_prev;
real32 L1Dist;
boolean GPSDataStale = false;

real32 EAS2TAS;

real32 alongTrackDist;
real32 omega;
real32 _bearing_error;

real32 DesiredLateralAcc;
real32 OrbitDirection;
boolean _reverse;
real32 omegaA;
real32 AB_length;
real32 A_air_unity, A_air_unitx;

real32 _groundspeed_roll, _groundspeed_pitch;

real32 ABx, ABy;
real32 B_air_unitx, B_air_unity;

real32 A_airlength;
real32 NuP;

// Wrap AHRS yaw if in reverse - radians

real32 L1_get_yaw(void) {
	if (_reverse)
		return MakePi(PI + Heading);

	return Heading;
} // L1_get_yaw

// return the bank angle needed to achieve tracking from the last
// update_*() operation

real32 L1_nav_roll(void) {
	real32 ret;
	ret = cosf(A[Pitch].Angle) * atanf(DesiredLateralAcc * GRAVITY_MPS_S_R);
	return Limit1(ret, HALF_PI);

} // L1_nav_roll

// this is the turn distance assuming a 90 degree turn
real32 _L1_turn_distance(real32 WPRadius) {

	WPRadius *= Sqr(EAS2TAS);
	return Min(WPRadius, L1Dist);
}

/*
 this approximates the turn distance for a given turn angle. If the
 turn_angle is > 90 then a 90 degree turn distance is used, otherwise
 the turn distance is reduced linearly.
 This function allows straight ahead mission legs to avoid thinking
 they have reached the waypoint early, which makes things like camera
 trigger and ball drop at exact positions under mission control much easier
 */
real32 L1_turn_distance(real32 WPRadius, real32 turn_angle) {
	real32 distance_90 = 123; //XXX_turn_distance(WPRadius);

	turn_angle = fabsf(turn_angle);
	if (turn_angle >= HALF_PI)
		return distance_90;

	return distance_90 * turn_angle / HALF_PI;
}

boolean L1_reached_loiter_target(void) {
	return F.WayPointCentred;
}

// prevent indecision in our turning by using our previous turn
// decision if we are in a narrow angle band pointing away from the
// target and the turn angle has changed sign

real32 L1PreventIndecision(real32 Nu) {
	// we are moving away from the target waypoint and pointing
	// away from the waypoint (not flying backwards). The sign
	// of Nu has also changed, which means we are
	// oscillating in our decision about which way to go
	const real32 Nu_limit = 0.9f * PI;

	if ((fabsf(Nu) > Nu_limit) && (fabsf(NuP) > Nu_limit)
			&& (Abs(MakePi(Nav.WPBearing - Heading)) > RadiansToDegrees(120))
			&& ((Nu * NuP) < 0.0f))
		Nu = NuP;

	return Nu;
} // L1PreventIndecision


void L1CrossTrack(void) {
	real32 Nu;
	real32 ForwardTrackVel, LateralTrackVel;

	// Calculate L1 gain required for specified damping
	real32 K_L1 = 4.0f * Sqr(L1Damping);

	// Get current position and velocity
	//if (!_ahrs.get_position(_current_loc)) {
	// if no GPS loc available, maintain last nav/target_bearing
	//	GPSDataStale = true;
	//	return;
	//}

	// Calculate time varying control parameters
	// Calculate the L1 length required for specified period
	L1Dist = (1.0f / PI) * L1Damping * L1Period * GPS.gspeed;

	// Calculate the NE position of WP B relative to WP A

	// Check for AB zero length and track directly to the destination
	// if too small
	//	if (AB.length() < 1.0e-6f) {
	//		AB = location_diff(_current_loc, next_WP);
	//		if (AB.length() < 1.0e-6f)
	//			AB = Vector2f(cosf(get_yaw()), sinf(get_yaw()));
	//	}
	//	AB.normalize();

	// Calculate the NE position of the aircraft relative to WP A

	// calculate distance to target track, for reporting

	//Determine if the aircraft is behind a +-135 degree degree arc centred on WP A
	//and further than L1 distance from WP A. Then use WP A as the L1 reference point
	//Otherwise do normal L1 guidance
	//real32 WP_A_dist = WPDistance(CurrWP - 1);
	real32 alongTrackDist = 123; //A_air * AB;
	if ((Nav.WPDistance > L1Dist) && (alongTrackDist
			/Max(Nav.WPDistance, 1.0f) < -0.7071f)) {
		//Calc Nu to fly To WP A
		//	Vector2f A_air_unit = (A_air).normalized; // Unit vector from WP A to aircraft
		ForwardTrackVel = _groundspeed_roll; // Velocity across line
		LateralTrackVel = _groundspeed_pitch; // Velocity along line
		Nu = atan2f(ForwardTrackVel, LateralTrackVel);
		Nav.DesiredHeading = atan2f(-A_air_unity, -A_air_unitx); // bearing (radians) from AC to L1 point
	} else if (alongTrackDist > AB_length + GPS.gspeed * 3.0f) {
		// we have passed point B by 3 seconds. Head towards B
		// Calc Nu to fly To WP B
		//	Vector2f B_air = location_diff(next_WP, _current_loc);
		//	Vector2f B_air_unit = (B_air).normalized; // Unit vector from WP B to aircraft
		ForwardTrackVel = _groundspeed_roll; // Velocity across line
		LateralTrackVel = _groundspeed_pitch; // Velocity along line
		Nu = atan2f(ForwardTrackVel, LateralTrackVel);
		Nav.DesiredHeading = atan2f(-B_air_unity, -B_air_unitx); // bearing (radians) from AC to L1 point

	} else { //Calc Nu to fly along AB line

		//Calculate Nu2 angle (angle of velocity vector relative to line connecting waypoints)
		ForwardTrackVel = 123; //_groundspeed_vector % AB; // Velocity cross track
		LateralTrackVel = 123; //_groundspeed_vector * AB; // Velocity along track
		real32 Nu2 = atan2f(ForwardTrackVel, LateralTrackVel);
		//Calculate Nu1 angle (Angle to L1 reference point)
		real32 sine_Nu1 = 123; //_crosstrack_error/MAX(L1Dist, 0.1f);
		//Limit sine of Nu1 to provide a controlled track capture angle of 45 deg
		sine_Nu1 = Limit1(sine_Nu1, sinf(DegreesToRadians(45)));
		real32 Nu1 = asinf(sine_Nu1);

		// compute integral error component to converge to a crosstrack of zero when traveling
		// straight but reset it when disabled or if it changes. That allows for much easier
		// tuning by having it re-converge each time it changes.
		if ((_L1_ForwardTrack_i_gain <= 0) || (_L1_ForwardTrack_i_gain
				!= _L1_ForwardTrack_i_gain_prev)) {
			_L1_ForwardTrack_i = 0;
			_L1_ForwardTrack_i_gain_prev = _L1_ForwardTrack_i_gain;
		} else if (fabsf(Nu1) < DegreesToRadians(5)) {
			_L1_ForwardTrack_i += Nu1 * _L1_ForwardTrack_i_gain * NavdT;
			// an AHRS_TRIM_X=0.1 will drift to about 0.08 so 0.1 is a good worst-case to clip at
			_L1_ForwardTrack_i = Limit1(_L1_ForwardTrack_i, 0.1f);
		}

		// to converge to zero we must push Nu1 harder
		Nu1 += _L1_ForwardTrack_i;

		Nu = Nu1 + Nu2;
		Nav.DesiredHeading = atan2f(ABy, ABx) + Nu1; // bearing (radians) from AC to L1 point
	}

	NuP = Nu = L1PreventIndecision(Nu);

	Nu = Limit1(Nu, HALF_PI);
	DesiredLateralAcc = K_L1 * Sqr(GPS.gspeed) / L1Dist * sinf(Nu);

	// Waypoint capture status is always false during waypoint following
	F.WayPointCentred = false;

	_bearing_error = Nu; // bearing error angle (radians), +ve to left of track

	GPSDataStale = false; // status are correctly updated with current waypoint data

} // L1CrossTrack


void L1_update_loiter(real32 radius, uint8 OrbitDirection) { //void) //Location *center_WP, real32 radius, int8 OrbitDirection)

	// scale loiter radius with square of EAS2TAS to allow us to stay
	// stable at high altitude
	radius *= Sqr(EAS2TAS);

	// Calculate guidance gains used by PD loop (used during circle tracking)
	real32 omega = (TWO_PI / L1Period);
	real32 Kx = Sqr(omega);
	real32 Kv = 2.0f * L1Damping * omega;

	// Calculate L1 gain required for specified damping (used during waypoint capture)
	real32 K_L1 = 4.0f * Sqr(L1Damping);

	//Get current position and velocity
	//	if (_ahrs.get_position(_current_loc) == false) {
	// if no GPS loc available, maintain last nav/target_bearing
	GPSDataStale = true;
	//		return;
	//	}

	//zzz	Vector2f _groundspeed_vector = _ahrs.groundspeed_vector;

	//Calculate groundspeed
	//zzz	real32 groundSpeed = Max(_groundspeed_vector.length , 1.0f);


	// Calculate time varying control parameters
	// Calculate the L1 length required for specified period
	L1Dist = (1.0f / PI) * L1Damping * L1Period * GPS.gspeed;

	//Calculate the NE position of the aircraft relative to WP A
	//	Vector2f A_air = location_diff(center_WP, _current_loc);

	// Calculate the unit vector from WP A to aircraft
	// protect against being on the waypoint and having zero velocity
	// if too close to the waypoint, use the velocity vector
	// if the velocity vector is too small, use the heading vector
	//	Vector2f A_air_unit;
	//	if (A_air.length > 0.1f)
	//		A_air_unit = A_air.normalized;
	//	else
	//		if (_groundspeed_vector.length < 0.1f)
	//			A_air_unit = Vector2f(cosf(_ahrs.yaw), sinf(L1_get_yaw()));
	//		else
	//			A_air_unit = _groundspeed_vector.normalized;


	//Calculate Nu to capture center_WP
	real32 ForwardTrackVelCap = 123; // A_air_unit % _groundspeed_vector; // Velocity across line - perpendicular to radial inbound to WP
	real32 LateralTrackVelCap = 123; // - (_groundspeed_vector * A_air_unit); // Velocity along line - radial inbound to WP
	real32 Nu = atan2f(ForwardTrackVelCap, LateralTrackVelCap);

	NuP = Nu = L1PreventIndecision(Nu);
	Nu = Limit1(Nu, HALF_PI);

	//Calculate lat accln demand to capture center_WP (use L1 guidance law)
	real32 LateralAccDemCap = K_L1 * Sqr(GPS.gspeed) / L1Dist * sinf(Nu);

	//Calculate radial position and velocity errors
	real32 ForwardTrackVelCirc = -LateralTrackVelCap; // Radial outbound velocity - reuse previous radial inbound velocity
	real32 ForwardTrackErrCirc = A_airlength - radius; // Radial distance from the loiter circle

	// keep crosstrack error for reporting
	Nav.CrossTrackE = ForwardTrackErrCirc;

	//Calculate PD control correction to circle waypoint_ahrs.roll
	real32 LateralAccDemCircPD = (ForwardTrackErrCirc * Kx
			+ ForwardTrackVelCirc * Kv);

	//Calculate tangential velocity
	real32 velTangent = ForwardTrackVelCap * OrbitDirection;

	//Prevent PD demand from turning the wrong way by limiting the command when flying the wrong way
	if ((LateralTrackVelCap < 0.0f) && (velTangent < 0.0f))
		LateralAccDemCircPD = Max(LateralAccDemCircPD, 0.0f);

	// Calculate centripetal acceleration demand
	real32 LateralAccDemCircCtr = velTangent * velTangent
			/ Max((0.5f * radius), (radius + ForwardTrackErrCirc));

	//Sum PD control and centripetal acceleration to calculate lateral manoeuvre demand
	real32 LateralAccDemCirc = OrbitDirection * (LateralAccDemCircPD
			+ LateralAccDemCircCtr);

	// Perform switchover between 'capture' and 'circle' modes at the
	// point where the commands cross over to achieve a seamless transfer
	// Only fly 'capture' mode if outside the circle
	if ((ForwardTrackErrCirc > 0.0f) && (OrbitDirection * LateralAccDemCap
			< OrbitDirection * LateralAccDemCirc)) {
		DesiredLateralAcc = LateralAccDemCap;
		F.WayPointCentred = false;
		_bearing_error = Nu; // angle between demanded and achieved velocity vector, +ve to left of track
		Nav.DesiredHeading = atan2f(-A_air_unity, -A_air_unitx); // bearing (radians) from AC to L1 point
	} else {
		DesiredLateralAcc = LateralAccDemCirc;
		F.WayPointCentred = true;
		_bearing_error = 0.0f; // bearing error (radians), +ve to left of track
		Nav.DesiredHeading = atan2f(-A_air_unity, -A_air_unitx); // bearing (radians)from AC to L1 point
	}

	GPSDataStale = false; // status are correctly updated with current waypoint data
} // L1_update_loiter


void L1_update_heading_hold(void) {
	// Calculate normalised frequency for tracking loop
	const real32 omegaA = 4.4428f / L1Period; // sqrt(2)*pi/period
	// Calculate additional damping gain

	real32 Nu;

	// copy to Nav.WPBearing and _nav_bearing
	Nav.DesiredHeading = MakePi(Nav.DesiredHeading);
	//	_nav_bearing = DegreesToRadians(navigation_heading_cd * 0.01f);

	Nu = MakePi(Nav.DesiredHeading - Heading);

	//Vector2f _groundspeed_vector = _ahrs.groundspeed_vector;

	//Calculate groundspeed
	//	real32 groundSpeed = _groundspeed_vector.length;

	// Calculate time varying control parameters
	L1Dist = GPS.gspeed / omegaA; // L1 distance is adjusted to maintain a constant tracking loop frequency
	real32 VomegaA = GPS.gspeed * omegaA;

	// Waypoint capture status is always false during heading hold
	F.WayPointCentred = false;

	Nav.CrossTrackE = 0.0f;

	_bearing_error = Nu; // bearing error angle (radians), +ve to left of track

	// Limit Nu to +-pi
	Nu = Limit1(Nu, PI);
	DesiredLateralAcc = 2.0f * sinf(Nu) * VomegaA;

	GPSDataStale = false; // status are correctly updated with current waypoint data

} // L1_update_heading_hold

//_______________________________________________________________________________

void CaptureWPHeading(void) {

	if (CurrWPNo != PrevWPNo) {
		Nav.OriginalWPBearing = Nav.WPBearing;
		PrevWPNo = CurrWPNo;
		F.CrossTrackActive = true;
	}

} // CaptureWPHeading

boolean UseCrossTrack(real32 DiffHeading) {

	boolean r = (((NavState == Transiting) || (NavState == AcquiringAltitude)
			|| (NavState == ReturningHome)) && (Nav.WPDistance
			> NV.Mission.ProximityRadius * 2.0f) && (Abs(MakePi(DiffHeading))
			< DegreesToRadians(45)));
	if (!r)
		Nav.CrossTrackPosE[NorthC] = Nav.CrossTrackPosE[EastC]
				= Nav.CrossTrackE = 0.0f;
	return r;
} // UseCrossTrack


void CompensateMulticopterCrossTrackError1D(void) {
	real32 DiffHeading;

	CaptureWPHeading();

	DiffHeading = Nav.WPBearing - Nav.OriginalWPBearing;
	if (UseCrossTrack(DiffHeading)) {
		Nav.CrossTrackE = sinf(DiffHeading) * Nav.WPDistance;
		Nav.CrossTrackPosE[EastC] = Nav.CrossTrackE * sin(Nav.WPBearing);
		Nav.CrossTrackPosE[NorthC] = Nav.CrossTrackE * cos(Nav.WPBearing);
	} else {
		Nav.OriginalWPBearing = Nav.WPBearing; // safety
		F.CrossTrackActive = false;
		Nav.CrossTrackPosE[NorthC] = Nav.CrossTrackPosE[EastC] = 0.0f;
	}

} // CompensateMulticopterCrossTrackError1D


void CompensateCrossTrackError1D(void) {
	real32 DiffHeading;

	CaptureWPHeading();

	Nav.CrossTrackPosE[NorthC] = Nav.CrossTrackPosE[EastC] = 0.0f;

	DiffHeading = Nav.WPBearing - Nav.OriginalWPBearing;
	if (UseCrossTrack(DiffHeading)) {
		Nav.CrossTrackE = sinf(DiffHeading) * Nav.WPDistance;
		Nav.WPBearing
				+= Limit1(Nav.CrossTrackE * Nav.CrossTrackKp, DegreesToRadians(30));
		Nav.WPBearing = Make2Pi(Nav.WPBearing);
	} else {
		Nav.OriginalWPBearing = Nav.WPBearing; // safety
		F.CrossTrackActive = false;
		Nav.CrossTrackE = 0.0f;
	}

} // CompensateCrossTrackError1D

void CheckProximity(void) {

	if (CurrGPSType == UBXBinGPS)
		F.WayPointCentred = IsFixedWing ? Nav.WPDistance
				< NV.Mission.ProximityRadius : Nav.WPDistance < (GPS.hAcc
				* 5.0f); // TODO: is 5 too large?
	else
		F.WayPointCentred = Nav.WPDistance < NV.Mission.ProximityRadius;

	F.WayPointAchieved = F.WayPointCentred && (Abs(DesiredAltitude - Altitude)
			< NV.Mission.ProximityAltitude);
} // CheckProximity

void DecayPosCorr(void) {
	static uint32 LastUpdateuS = 0;
	real32 dT, Decay;
	int32 a;

	dT = dTUpdate(uSClock(), &LastUpdateuS);
	Decay = NAV_CORR_DECAY;

	for (a = Pitch; a <= Yaw; a++) {
		A[a].NavCorr = DecayX(A[a].NavCorr, Decay, dT);
		Nav.CorrP[a] = Nav.VelEP[a] = Nav.VelIntE[a] = 0.0f;
	}

	F.WayPointAchieved = F.WayPointCentred = false;
} // DecayPosCorr

void DoOrbit(real32 Radius, real32 OrbitVelocity) {
	real32 TangentialVelocity;

	TangentialVelocity = (Nav.WPDistance - Radius) * Nav.O.Kp * Nav.Sensitivity;

	Rotate(&Nav.DesVel[NorthC], &Nav.DesVel[EastC], TangentialVelocity,
			OrbitVelocity, -Heading);

} // DoOrbit


void ComputeRelVelocity2D(int32 a) {
	// use position difference to compute velocity when travelling slowly
	// including close to WP
	real32 D;

	if (Nav.WPDistance < NV.Mission.ProximityRadius)
		Nav.PosE[a] = SlewLimitLPFilter(Nav.PosEP[a], Nav.PosE[a], 5.0f,
				Nav.LPFCutOffHz, NavdT);

	D = Nav.PosEP[a] - Nav.PosE[a];
	Nav.PosEP[a] = Nav.PosE[a];

	// GPS velocity only useful at higher speeds
	Nav.Vel[a] = Abs(GPS.gspeed) < 4.0f ? D * NavdTR : SlewLimitLPFilter(
			Nav.VelP[a], Nav.Vel[a], 10.0f, Nav.LPFCutOffHz, NavdT);
	Nav.VelP[a] = Nav.Vel[a];

	Nav.Vel[a] = Threshold(Nav.Vel[a], 0.1f);

} // ComputeRelVelocity

real32 NavCorr[2];

real32 WPDistance(WPStruct * W) {
	real32 NorthE, EastE;

	NorthE = W->Pos[NorthC] - Nav.Pos[NorthC];
	EastE = W->Pos[EastC] - Nav.Pos[EastC];

	return sqrtf(Sqr(EastE) + Sqr(NorthE));

} // WPDistance

void Navigate(WPStruct * W) {
	int32 a;
	real32 Pp, Pv, Iv;
	real32 POIEastDiff, POINorthDiff, POIDistance;
	static boolean Saturating[2] = { false };
	real32 VelScale[2];

	NavdT = dTUpdate(uSClock(), &LastNavUpdateuS);
	NavdTR = 1.0f / NavdT;

	Nav.Desired[NorthC] = W->Pos[NorthC];
	Nav.Desired[EastC] = W->Pos[EastC];

	Nav.PosE[NorthC] = Nav.Desired[NorthC] - Nav.Pos[NorthC];
	Nav.PosE[EastC] = Nav.Desired[EastC] - Nav.Pos[EastC];

	Nav.WPDistance = sqrtf(Sqr(Nav.PosE[EastC]) + Sqr(Nav.PosE[NorthC]));
	Nav.WPBearing = Make2Pi(atan2f(Nav.PosE[EastC], Nav.PosE[NorthC]));

	if (ResetNavHold) {
		ResetNavHold = F.RapidDescentHazard = F.WayPointAchieved
				= F.WayPointCentred = false;
		Nav.VelocityLimit = NAV_MIN_VEL_MPS;
		for (a = NorthC; a <= EastC; a++) {
			Nav.VelIntE[a] = Nav.VelP[a] = 0.0f;
			Nav.PosEP[a] = Nav.PosE[a];
		}
	}

	CheckProximity();

	Nav.KpEff = Nav.I.Kp * Nav.Sensitivity;
	Nav.KiEff = Limit(0.2061 * Nav.KpEff - 0.0104f, 0.0f, Nav.I.Ki);

	if (IsFixedWing) {

		CompensateCrossTrackError1D();

		A[Pitch].NavCorr = A[Yaw].NavCorr = 0.0f;
		Nav.DesiredHeading = MakePi(Nav.WPBearing);
		// control is by yaw rate in control.c

	} else {
#define USE_FW_XTRACK
#if defined(USE_FW_XTRACK)
		CompensateCrossTrackError1D();
#else
		CompensateMulticopterCrossTrackError1D();
#endif

		Nav.VelocityLimit // TODO: questionable given slew limiting on corrections?
				= Limit(Nav.VelocityLimit + 0.25f * NavdT, NAV_MIN_VEL_MPS, Nav.MaxVelocity);

		if (F.RapidDescentHazard)
			DoOrbit(DESCENT_RADIUS_M, DESCENT_VELOCITY_MPS);
		else {
			if (F.OrbitingWP)
				DoOrbit(W->OrbitRadius, W->OrbitVelocity);
		}

		VelScale[NorthC] = Abs(cosf(Nav.WPBearing));
		VelScale[EastC] = Abs(sinf(Nav.WPBearing));

		for (a = NorthC; a <= EastC; a++) {
			// Position
			Pp = F.OrbitingWP || F.RapidDescentHazard ? Nav.DesVel[a]
					: Nav.PosE[a] * Nav.O.Kp + Nav.CrossTrackPosE[a]
							* Nav.CrossTrackKp; // radius is MaxVel/Kp

			Nav.DesiredVel = Limit1(Pp, Nav.VelocityLimit * VelScale[a]);

			// Velocity
			ComputeRelVelocity2D(a);

			Nav.VelE[a] = Nav.DesiredVel - Nav.Vel[a];
			Nav.VelE[a] = SlewLimit(Nav.VelEP[a], Nav.VelE[a],
					NAV_MAX_ACC_MPSPS, NavdT);
			Nav.VelEP[a] = Nav.VelE[a];

			Pv = Nav.VelE[a] * Nav.KpEff;

			Nav.VelIntE[a] += Nav.VelE[a] * Nav.KiEff * NavdT;
			Nav.VelIntE[a] = Limit1(Nav.VelIntE[a], Nav.MaxAngle * VelScale[a]);

			Iv = Nav.VelIntE[a];

			NavCorr[a] = Pv + Iv;
			Saturating[a] = Abs(NavCorr[a]) > Nav.MaxAngle; // for info!
			if (Saturating[a])
				NavCorr[a] = Limit1(NavCorr[a], Nav.MaxAngle);

			NavCorr[a] = SlewLimit(Nav.CorrP[a], NavCorr[a],
					NAV_ATTITUDE_SLEW_RAD_S, NavdT);
			Nav.CorrP[a] = NavCorr[a];
		}

		A[Yaw].NavCorr = 0.0f;

		F.NavSaturation = Saturating[Pitch] || Saturating[Roll];

		Rotate(&A[Pitch].NavCorr, &A[Roll].NavCorr, -NavCorr[NorthC],
				NavCorr[EastC], -Heading);

		if (F.OrbitingWP || F.RapidDescentHazard)
			Nav.DesiredHeading = Nav.WPBearing;
		else {
			if (F.UsingPOI) {
				POIEastDiff = POI.Pos[EastC] - Nav.Pos[EastC];
				POINorthDiff = POI.Pos[NorthC] - Nav.Pos[NorthC];

				POIDistance = sqrtf(Sqr(POIEastDiff) + Sqr(POINorthDiff));
				Nav.DesiredHeading = (POIDistance > (NV.Mission.ProximityRadius
						* 2.0f)) ? atan2f(POIEastDiff, POINorthDiff) : Heading;
			} else {

				// Manual yaw control only

			}
		}
	}
} // Navigate


void InitNavigation(void) {
	int32 a;
	//gke
	//DEFINITELY not memset(&Nav, 0, sizeof(NavStruct));

	for (a = Pitch; a <= Yaw; a++)
		A[a].NavCorr = Nav.CorrP[a] = Nav.VelP[a] = Nav.VelEP[a]
				= Nav.VelIntE[a] = 0.0f;

	Nav.KpEff = Nav.KiEff = 1.0f;

	Nav.Elevation = Nav.Bearing = Nav.Distance = Nav.TakeoffBearing
			= Nav.WPDistance = Nav.WPBearing = Nav.CrossTrackE = 0.0f;

	Nav.CrossTrackPosE[NorthC] = Nav.CrossTrackPosE[EastC] = 0.0f;

	if (!F.OriginValid || F.Emulation) {
		GPS.OriginRaw[NorthC] = DEFAULT_HOME_LAT;
		GPS.OriginRaw[EastC] = DEFAULT_HOME_LON;
		GPS.longitudeCorrection = DEFAULT_LON_CORR;
		if (F.Emulation)
			GenerateNavTestMission();
	} else {
		NV.Mission.NoOfWayPoints = 0;
		NV.Mission.OriginAltitude = OriginAltitude;
		NV.Mission.RTHAltHold = (int16) (P(NavRTHAlt)); // ??? not used
	}

	NavState = PIC;

	POI.Pos[EastC] = POI.Pos[NorthC] = 0.0f;

	AttitudeHoldResetCount = 0;
	F.OriginValid = F.NavigationEnabled = F.NavigationActive
			= F.CrossTrackActive = F.WayPointAchieved = F.WayPointCentred
					= F.NewNavUpdate = false;

	A[Pitch].NavCorr = A[Roll].NavCorr = 0.0f;

	F.OrbitingWP = F.RapidDescentHazard = F.UsingPOI = false;
	CurrWPNo = 0;
	PrevWPNo = 255;
	NorthP = EastP = 0.0f; // origin
	RefreshNavWayPoint();
	DesiredAltitude = 0.0f;

} // InitNavigation

//#define INAV
#if defined(INAV)

#define US2S(t) ((real32)t * 1.0e-6)

/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

// If we are going slower than NAV_FW_MIN_VEL_SPEED_BOOST - boost throttle to fight against the wind
#define NAV_FW_THROTTLE_SPEED_BOOST_GAIN        1.5f
#define NAV_FW_MIN_VEL_SPEED_BOOST              7.0f      // 7 m/s
// If this is enabled navigation won't be applied if velocity is below 3 m/s
//#define NAV_FW_LIMIT_MIN_FLY_VELOCITY

static boolean isPitchAdjustmentValid = false;
static boolean isRollAdjustmentValid = false;
static real32 throttleSpeedAdjustment = 0;

/*-----------------------------------------------------------
 * Altitude controller
 *-----------------------------------------------------------*/
void setupFixedWingAltitudeController(void) {
	// TODO
}

void resetFixedWingAltitudeController() {
	navPidReset(&posControl.pids.fw_alt);
	posControl.rcAdjustment[PITCH] = 0;
	isPitchAdjustmentValid = false;
	throttleSpeedAdjustment = 0;
}

bool adjustFixedWingAltitudeFromRCInput(void) {
	real32 rcAdjustment = applyDeadband(rcCommand[PITCH],
			rcControlsConfig()->alt_hold_deadband);

	if (rcAdjustment) {
		// set velocity proportional to stick movement
		real32 rcClimbRate = -rcAdjustment
				* navConfig()->general.max_manual_climb_rate / (5.0f
				- rcControlsConfig()->alt_hold_deadband);
		updateAltitudeTargetFromClimbRate(rcClimbRate,
				CLIMB_RATE_RESET_SURFACE_TARGET);
		return true;
	} else
		return false;
}

// Position to velocity controller for Z axis
static void updateAltitudeVelocityAndPitchController_FW(uint32 dTuS) {
	static pt1Filter_t velzFilterState;

	// On a fixed wing we might not have a reliable climb rate source (if no BARO available), so we can't apply PID controller to
	// velocity error. We use PID controller on altitude error and calculate desired pitch angle from desired climb rate and forward velocity

	// FIXME: Use airspeed here
	real32 forwardVelocity = MAX(posControl.actualState.velXY, 3.0f); // Limit min velocity for PID controller at about 10 km/h

	// Calculate max climb rate from current forward velocity and maximum pitch angle (climb angle is fairly small, approximate tan=sin)
	real32 maxVelocityClimb = forwardVelocity * sinf(
			(navConfig()->fw.max_climb_angle));
	real32 maxVelocityDive = -forwardVelocity * sinf(
			(navConfig()->fw.max_dive_angle));

	posControl.desiredState.vel.V.Z = navPidApply2(&posControl.pids.fw_alt,
			posControl.desiredState.pos.V.Z, posControl.actualState.pos.V.Z,
			US2S(dTuS), maxVelocityDive, maxVelocityClimb, 0);
	posControl.desiredState.vel.V.Z = pt1FilterApply4(&velzFilterState,
			posControl.desiredState.vel.V.Z, NAV_FW_VEL_CUTOFF_FREQENCY_HZ,
			US2S(dTuS));

	// Calculate climb angle ( >0 - climb, <0 - dive)
	int16 climbAngleDeciDeg = (atan2_approx(posControl.desiredState.vel.V.Z,
			forwardVelocity));
	climbAngleDeciDeg
			= Limit(climbAngleDeciDeg, -navConfig()->fw.max_dive_angle * 10, navConfig()->fw.max_climb_angle * 10);
	posControl.rcAdjustment[PITCH] = climbAngleDeciDeg;

#if defined(NAV_BLACKBOX)
	navDesiredVelocity[Z] = Limit(posControl.desiredState.vel.V.Z, -32678, 32767);
	navTargetPosition[Z] = Limit(posControl.desiredState.pos.V.Z, -32678, 32767);
#endif
}

void applyFixedWingAltitudeController(uint32 NowuS) {
	static uint32 PrevPosUpdateuS; // Occurs @ altitude sensor update rate (max MAX_ALTITUDE_UPDATE_RATE_HZ)
	static uint32 PrevUpdateuS; // Occurs @ looptime rate

	const uint32 dTuS = NowuS - PrevUpdateuS;
	PrevUpdateuS = NowuS;

	// If last time Z-controller was called is too far in the past - ignore it (likely restarting altitude controller)
	if (dTuS > HZ2US(MIN_POSITION_UPDATE_RATE_HZ)) {
		PrevUpdateuS = NowuS;
		PrevPosUpdateuS = NowuS;
		resetFixedWingAltitudeController();
		return;
	}

	if (posControl.flags.hasValidPositionSensor) {
		// If we have an update on vertical position data - update velocity and accel targets
		if (posControl.flags.verticalPositionDataNew) {
			const uint32 dTuSPositionUpdate = NowuS - PrevPosUpdateuS;
			PrevPosUpdateuS = NowuS;

			// Check if last correction was too log ago - ignore this update
			if (dTuSPositionUpdate < HZ2US(MIN_POSITION_UPDATE_RATE_HZ))
				updateAltitudeVelocityAndPitchController_FW(dTuSPositionUpdate);
			else
				// due to some glitch position update has not occurred in time, reset altitude controller
				resetFixedWingAltitudeController();

			// Indicate that information is no longer usable
			posControl.flags.verticalPositionDataConsumed = 1;
		}

		isPitchAdjustmentValid = true;
	} else
		// No valid altitude sensor data, don't adjust pitch automatically, rcCommand[PITCH] is passed through to PID controller
		isPitchAdjustmentValid = false;
}

/*-----------------------------------------------------------
 * Adjusts desired heading from pilot's input
 *-----------------------------------------------------------*/
boolean adjustFixedWingHeadingFromRCInput(void) {
	return false;
}

/*-----------------------------------------------------------
 * XY-position controller for multicopter aircraft
 *-----------------------------------------------------------*/
static t_fp_vector VWP;
static pt1Filter_t fwPosControllerCorrectionFilterState;

void resetFixedWingPositionController(void) {
	VWP.V.X = 0;
	VWP.V.Y = 0;
	VWP.V.Z = 0;

	navPidReset(&posControl.pids.fw_nav);
	posControl.rcAdjustment[ROLL] = 0;
	isRollAdjustmentValid = false;

	pt1FilterReset(&fwPosControllerCorrectionFilterState, 0.0f);
}

static void calculateVirtualPositionTarget_FW(real32 trackingPeriod) {
	real32 posErrorX = posControl.desiredState.pos.V.X
			- posControl.actualState.pos.V.X;
	real32 posErrorY = posControl.desiredState.pos.V.Y
			- posControl.actualState.pos.V.Y;

	real32 distanceToActualTarget = sqrtf(sq(posErrorX) + sq(posErrorY));

	// Limit minimum forward velocity to 1 m/s
	real32 trackingDistance = trackingPeriod * MAX(
			posControl.actualState.velXY, 1.0f);

	// If angular visibility of a waypoint is less than 30deg, don't calculate circular loiter, go straight to the target
#define TAN_15DEG    0.26795f
	boolean needToCalculateCircularLoiter = isApproachingLastWaypoint()
			&& (distanceToActualTarget <= (navConfig()->fw.loiter_radius
					/ TAN_15DEG)) && (distanceToActualTarget > 0.5f);

	// Calculate virtual position for straight movement
	if (needToCalculateCircularLoiter) {
		// We are closing in on a waypoint, calculate circular loiter
		real32 loiterAngle = atan2_approx(-posErrorY, -posErrorX)
				+ DegreesToRadians(45);

		real32 loiterTargetX = posControl.desiredState.pos.V.X
				+ navConfig()->fw.loiter_radius * cosf(loiterAngle);
		real32 loiterTargetY = posControl.desiredState.pos.V.Y
				+ navConfig()->fw.loiter_radius * sinf(loiterAngle);

		// We have temporary loiter target. Recalculate distance and position error
		posErrorX = loiterTargetX - posControl.actualState.pos.V.X;
		posErrorY = loiterTargetY - posControl.actualState.pos.V.Y;
		distanceToActualTarget = sqrtf(sq(posErrorX) + sq(posErrorY));
	}

	// Calculate virtual waypoint
	VWP.V.X = posControl.actualState.pos.V.X + posErrorX * (trackingDistance
			/ distanceToActualTarget);
	VWP.V.Y = posControl.actualState.pos.V.Y + posErrorY * (trackingDistance
			/ distanceToActualTarget);

	// Shift position according to pilot's ROLL input (up to max_manual_speed velocity)
	if (posControl.flags.isAdjustingPosition) {
		real32 rcRollAdjustment = applyDeadband(rcCommand[ROLL],
				rcControlsConfig()->pos_hold_deadband);

		if (rcRollAdjustment) {
			real32 rcShiftY = rcRollAdjustment
					* navConfig()->general.max_manual_speed / 5.0f
					* trackingPeriod;

			// Rotate this target shift from body frame to to earth frame and apply to position target
			VWP.V.X += -rcShiftY * posControl.actualState.sinYaw;
			VWP.V.Y += rcShiftY * posControl.actualState.cosYaw;

			posControl.flags.isAdjustingPosition = true;
		}
	}
}

boolean adjustFixedWingPositionFromRCInput(void) {
	real32 rcRollAdjustment = applyDeadband(rcCommand[ROLL],
			rcControlsConfig()->pos_hold_deadband);
	return (rcRollAdjustment);
}

static void updatePositionHeadingController_FW(uint32 NowuS, uint32 dTuS) {
	static uint32 PrevMonitoringUpdateuS;
	static real32 HeadingEp;
	static boolean DecreasingHeadingE;
	static boolean forceTurnDirection = false;

	// We have virtual position target, calculate heading error
	int32_t virtualTargetBearing = calculateBearingToDestination(&VWP);

	// Calculate NAV heading error
	real32 HeadingE = MakePi(virtualTargetBearing - posControl.actualState.yaw);

	// Forced turn direction
	// If heading error is close to 180 deg we initiate forced turn and only disable it when heading error goes below 90 deg
	if (Abs(HeadingE) > DegreesToRadians(170))
		forceTurnDirection = true;
	else if (Abs(HeadingE) < DegreesToRadians(90) && forceTurnDirection)
		forceTurnDirection = false;

	// If forced turn direction flag is enabled we fix the sign of the direction
	if (forceTurnDirection)
		HeadingE = Abs(HeadingE);

	// Slow error monitoring (2Hz rate)
	if ((NowuS - PrevMonitoringUpdateuS) >= HZ2US(
			NAV_FW_CONTROL_MONITORING_RATE)) {
		// Check if error is decreasing over time
		DecreasingHeadingE = Abs(HeadingEp) > Abs(HeadingE);

		// Save values for next iteration
		HeadingEp = HeadingE;
		PrevMonitoringUpdateuS = NowuS;
	}

	// Only allow PID integrator to shrink if error is decreasing over time
	const pidControllerFlags_e pidFlags = PID_DTERM_FROM_ERROR
			| (DecreasingHeadingE ? PID_SHRINK_INTEGRATOR : 0);

	// Input error in (deg*100), output pitch angle (deg*100)
	real32 rollAdjustment = navPidApply2(&posControl.pids.fw_nav,
			posControl.actualState.yaw + HeadingE, posControl.actualState.yaw,
			US2S(dTuS), -(navConfig()->fw.max_bank_angle),
			(navConfig()->fw.max_bank_angle), pidFlags);

	// Apply low-pass filter to prevent rapid correction
	rollAdjustment = pt1FilterApply4(&fwPosControllerCorrectionFilterState,
			rollAdjustment, NAV_FW_ROLL_CUTOFF_FREQUENCY_HZ, US2S(dTuS));

	// Convert rollAdjustment to decidegrees (rcAdjustment holds decidegrees)
	posControl.rcAdjustment[ROLL] = rollAdjustment;

	// Update magHold heading lock in case pilot is using MAG mode (prevent MAGHOLD to fight navigation)
	posControl.desiredState.yaw
			= Make2Pi(posControl.actualState.yaw + HeadingE);
	updateMagHoldHeading(posControl.desiredState.yaw);

	// Add pitch compensation
	//posControl.rcAdjustment[PITCH] = -(Abs(rollAdjustment)) * 0.50f;
}

void applyFixedWingPositionController(uint32 NowuS) {
	static uint32 PrevPosUpdateuS; // Occurs @ GPS update rate
	static uint32 PrevUpdateuS; // Occurs @ looptime rate

	const uint32 dTuS = NowuS - PrevUpdateuS;
	PrevUpdateuS = NowuS;

	// If last position update was too long in the past - ignore it (likely restarting altitude controller)
	if (dTuS > HZ2US(MIN_POSITION_UPDATE_RATE_HZ)) {
		PrevUpdateuS = NowuS;
		PrevPosUpdateuS = NowuS;
		resetFixedWingPositionController();
		return;
	}

	// Apply controller only if position source is valid. In absence of valid pos sensor (GPS loss), we'd stick in forced ANGLE mode
	if (posControl.flags.hasValidPositionSensor) {
		// If we have new position - update velocity and acceleration controllers
		if (posControl.flags.horizontalPositionDataNew) {
			const uint32 dTuSPositionUpdate = NowuS - PrevPosUpdateuS;
			PrevPosUpdateuS = NowuS;

			if (dTuSPositionUpdate < HZ2US(MIN_POSITION_UPDATE_RATE_HZ)) {
				// Calculate virtual position target at a distance of forwardVelocity * HZ2S(POSITION_TARGET_UPDATE_RATE_HZ)
				// Account for pilot's roll input (move position target left/right at max of max_manual_speed)
				// POSITION_TARGET_UPDATE_RATE_HZ should be chosen keeping in mind that position target shouldn't be reached until next pos update occurs
				// FIXME: verify the above
				calculateVirtualPositionTarget_FW(HZ2S(
						MIN_POSITION_UPDATE_RATE_HZ) * 2);

				updatePositionHeadingController_FW(NowuS, dTuSPositionUpdate);
			} else
				resetFixedWingPositionController();

			// Indicate that information is no longer usable
			posControl.flags.horizontalPositionDataConsumed = 1;
		}

		isRollAdjustmentValid = true;
	} else
		// No valid pos sensor data, don't adjust pitch automatically, rcCommand[ROLL] is passed through to PID controller
		isRollAdjustmentValid = false;
}

int16 applyFixedWingMinSpeedController(uint32 NowuS) {
	static uint32 PrevPosUpdateuS; // Occurs @ GPS update rate
	static uint32 PrevUpdateuS; // Occurs @ looptime rate

	const uint32 dTuS = NowuS - PrevUpdateuS;
	PrevUpdateuS = NowuS;

	// If last position update was too long in the past - ignore it (likely restarting altitude controller)
	if (dTuS > HZ2US(MIN_POSITION_UPDATE_RATE_HZ)) {
		PrevUpdateuS = NowuS;
		PrevPosUpdateuS = NowuS;
		throttleSpeedAdjustment = 0;
		return 0;
	}

	// Apply controller only if position source is valid
	if (posControl.flags.hasValidPositionSensor) {
		// If we have new position - update velocity and acceleration controllers
		if (posControl.flags.horizontalPositionDataNew) {
			const uint32 dTuSPositionUpdate = NowuS - PrevPosUpdateuS;
			PrevPosUpdateuS = NowuS;

			if (dTuSPositionUpdate < HZ2US(MIN_POSITION_UPDATE_RATE_HZ)) {
				real32 velThrottleBoost = (NAV_FW_MIN_VEL_SPEED_BOOST
						- posControl.actualState.velXY)
						* NAV_FW_THROTTLE_SPEED_BOOST_GAIN * US2S(
						dTuSPositionUpdate);

				// If we are in the deadband of 50cm/s - don't update speed boost
				if (Abs(posControl.actualState.velXY - NAV_FW_MIN_VEL_SPEED_BOOST)
						> 0.5f) {
					throttleSpeedAdjustment += velThrottleBoost;
				}

				throttleSpeedAdjustment
						= Limit(throttleSpeedAdjustment, 0.0f, 5.0f);
			} else
				throttleSpeedAdjustment = 0;

			// Indicate that information is no longer usable
			posControl.flags.horizontalPositionDataConsumed = 1;
		}
	} else
		// No valid pos sensor data, we can't calculate speed
		throttleSpeedAdjustment = 0;

	return throttleSpeedAdjustment;
}

void applyFixedWingPitchRollThrottleController(
		navigationFSMStateFlags_t navStateFlags, uint32 NowuS) {
	int16 pitchCorrection = 0; // >0 climb, <0 dive
	int16 rollCorrection = 0; // >0 right, <0 left
	int16 throttleCorrection = 0; // raw throttle

	int16 minThrottleCorrection = navConfig()->fw.min_throttle
			- navConfig()->fw.cruise_throttle;
	int16 maxThrottleCorrection = navConfig()->fw.max_throttle
			- navConfig()->fw.cruise_throttle;

	// Mix Pitch/Roll/Throttle
	if (isRollAdjustmentValid && (navStateFlags & NAV_CTL_POS)) {
		pitchCorrection += Abs(posControl.rcAdjustment[ROLL])
				* (navConfig()->fw.roll_to_pitch / 100.0f);
		rollCorrection += posControl.rcAdjustment[ROLL];
	}

	if (isPitchAdjustmentValid && (navStateFlags & NAV_CTL_ALT)) {
		pitchCorrection += posControl.rcAdjustment[PITCH];
		throttleCorrection += pitchCorrection
				* navConfig()->fw.pitch_to_throttle;
		throttleCorrection
				= Limit(throttleCorrection, minThrottleCorrection, maxThrottleCorrection);
	}

	// Speed controller - only apply in POS mode
	if (navStateFlags & NAV_CTL_POS) {
		throttleCorrection += applyFixedWingMinSpeedController(NowuS);
		throttleCorrection
				= Limit(throttleCorrection, minThrottleCorrection, maxThrottleCorrection);
	}

	// Limit and apply
	if (isPitchAdjustmentValid && (navStateFlags & NAV_CTL_ALT)) {
		// PITCH angle is measured in opposite direction ( >0 - dive, <0 - climb)
		pitchCorrection
				= Limit(pitchCorrection, -(navConfig()->fw.max_dive_angle), (navConfig()->fw.max_climb_angle));
		rcCommand[PITCH] = -pidAngleToRcCommand(pitchCorrection,
				pidProfile()->max_angle_inclination[FD_PITCH]);
	}

	if (isRollAdjustmentValid && (navStateFlags & NAV_CTL_POS)) {
		rollCorrection
				= Limit1(rollCorrection, (navConfig()->fw.max_bank_angle));
		rcCommand[ROLL] = pidAngleToRcCommand(rollCorrection,
				pidProfile()->max_angle_inclination[FD_ROLL]);

		// Calculate coordinated turn rate based on velocity and banking angle
		if (posControl.actualState.velXY >= 3.0f) {
			real32 targetYawRateDPS = (tan_approx(DECI(-rollCorrection))
					* GRAVITY_CMSS / posControl.actualState.velXY);
			rcCommand[YAW] = pidRateToRcCommand(targetYawRateDPS,
					currentControlRateProfile->rates[FD_YAW]);
		} else
			rcCommand[YAW] = 0;
	}

	if ((navStateFlags & NAV_CTL_ALT) || (navStateFlags & NAV_CTL_POS)) {
		uint16
				correctedThrottleValue =
						Limit(navConfig()->fw.cruise_throttle + throttleCorrection, navConfig()->fw.min_throttle, navConfig()->fw.max_throttle);
		rcCommand[THROTTLE]
				= Limit(correctedThrottleValue, motorConfig()->minthrottle, motorConfig()->maxthrottle);
	}
}

/*-----------------------------------------------------------
 * FixedWing land detector
 *-----------------------------------------------------------*/
static uint32 landingTimerUs;

void resetFixedWingLandingDetector(void) {
	landingTimerUs = uSClock();
}

boolean isFixedWingLandingDetected(void) {
	uint32 NowuS = uSClock();

	landingTimerUs = NowuS;
	return false;
}

/*-----------------------------------------------------------
 * FixedWing emergency landing
 *-----------------------------------------------------------*/
void applyFixedWingEmergencyLandingController(void) {
	// FIXME: Use altitude controller if available (similar to MC code)
	rcCommand[ROLL] = pidAngleToRcCommand(
			failsafeConfig()->failsafe_fw_roll_angle,
			pidProfile()->max_angle_inclination[FD_ROLL]);
	rcCommand[PITCH] = pidAngleToRcCommand(
			failsafeConfig()->failsafe_fw_pitch_angle,
			pidProfile()->max_angle_inclination[FD_PITCH]);
	rcCommand[YAW] = pidRateToRcCommand(failsafeConfig()->failsafe_fw_yaw_rate,
			currentControlRateProfile->rates[FD_YAW]);
	rcCommand[THROTTLE] = failsafeConfig()->failsafe_throttle;
}

/*-----------------------------------------------------------
 * Calculate loiter target based on current position and velocity
 *-----------------------------------------------------------*/
void calculateFixedWingInitialHoldPosition(t_fp_vector * pos) {
	// TODO: stub, this should account for velocity and target loiter radius
	*pos = posControl.actualState.pos;
}

void resetFixedWingHeadingController(void) {
	updateMagHoldHeading((posControl.actualState.yaw));
}

void applyFixedWingNavigationController(
		navigationFSMStateFlags_t navStateFlags, uint32 NowuS) {
	if (navStateFlags & NAV_CTL_LAUNCH)
		applyFixedWingLaunchController(NowuS);
	else if (navStateFlags & NAV_CTL_EMERG)
		applyFixedWingEmergencyLandingController();
	else {
#ifdef NAV_FW_LIMIT_MIN_FLY_VELOCITY
		// Don't apply anything if ground speed is too low (<3m/s)
		if (posControl.actualState.velXY > 300) {
			if (navStateFlags & NAV_CTL_ALT)
			applyFixedWingAltitudeController(NowuS);

			if (navStateFlags & NAV_CTL_POS)
			applyFixedWingPositionController(NowuS);
		}
		else {
			posControl.rcAdjustment[PITCH] = 0;
			posControl.rcAdjustment[ROLL] = 0;
		}
#else
		if (navStateFlags & NAV_CTL_ALT)
			applyFixedWingAltitudeController(NowuS);

		if (navStateFlags & NAV_CTL_POS)
			applyFixedWingPositionController(NowuS);
#endif

		//if (navStateFlags & NAV_CTL_YAW)
		if ((navStateFlags & NAV_CTL_ALT) || (navStateFlags & NAV_CTL_POS))
			applyFixedWingPitchRollThrottleController(navStateFlags, NowuS);
	}
}

#endif  // NAV
