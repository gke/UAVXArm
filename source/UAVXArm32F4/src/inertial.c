// ===============================================================================================
// =                                UAVX Quadrocopter ContRoller                                 =
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

//    You should have received a copy of the GNU General Public License aint32 with this program.
//    If not, see http://www.gnu.org/licenses/


#include "UAVX.h"

#define MAX_MAG_YAW_RATE_RADPS DegreesToRadians(60) // TODO: 180 may be too high - above this rate AHRS compensation of heading is zero
real32 dT, dTR, dTOn2, dTROn2;
uint32 LastInertialUpdateuS = 0;
real32 AccConfidenceSDevR = 5.0f;
real32 AccConfidence;
real32 AccZ;
real32 MagConfidence;
real32 EstMagHeading = 0.0f;

uint8 CurrStateEst = EstUnknown;

real32 CalculateAccConfidence(real32 AccMag) {
	// Gaussian decay in accelerometer value belief
	static real32 confp = 1.0f;
	real32 conf, accNorm;

	accNorm = AccMag * GRAVITY_MPS_S_R;
	conf = expf(-0.5f * Sqr(Abs(accNorm - 1.0f) * AccConfidenceSDevR));
	confp = HardFilter(confp, conf);

	return (confp);
} // CalculateAccConfidence


real32 CalculateMagConfidence(void) {
	// reduce confidence in mag for high yaw rates
	real32 c;

	//	xxx add 11% shift from initial mag?
	// MagLockE ???


	if (State == InFlight) {
		c = KpMagBase * (1.0f - Rate[Yaw] / MAX_MAG_YAW_RATE_RADPS); // linear for now
		c = Limit(c, 0.0f, KpMagBase);
	} else
		c = KpMagBase * 5.0f;

	return (c);
} // CalculateMagConfidence

//=====================================================================================================
// AHRS
// S.O.H. Madgwick
// 25th August 2010
//=====================================================================================================
// Description:
//
// Quaternion implementation of the 'DCM filter' [Mayhony et al].  Incorporates the magnetic distortion
// compensation algorithms from my filter [Madgwick] which eliminates the need for a reference
// direction of flux (bx bz) to be predefined and limits the effect of magnetic distortions to yaw
// axis only.
//
// Gyroscope units are radians/second, accelerometer and magnetometer units are irrelevant as the
// vector is normalised.
//
// adapted from John Ihlein's AQ version translated to Aerospace Coordinates
//
// integral terms removed and accelerometer & magnetometer confidence scheme by Prof. G.K. Egan 2012
//
//=====================================================================================================

real32 KpAccBase = 2.0f; // was 5.0f
real32 KpAcc;
real32 KpMagBase = 5.0f;
real32 KpMag;

//#define gyroMeasError DegreesToRadians(5)  // gyroscope measurement error in rad/s (shown as 5 deg/s)
//#define gyroMeasDrift DegreesToRadians(0.2) // gyroscope measurement error in rad/s/s (shown as 0.2f deg/s/s)
//#define beta (sqrtf(0.75f)*gyroMeasError) // compute beta aka KpAccBase
//#define zeta (sqrtf(0.75f)*gyroMeasDrift) // compute zeta aka KiAccBase not used

real32 q0, q1, q2, q3;
real32 q0q0, q0q1, q0q2, q0q3;
real32 q1q1, q1q2, q1q3;
real32 q2q2, q2q3;
real32 q3q3;

real32 bi00, bi01, bi02;
real32 bi10, bi11, bi12;
real32 bi20, bi21, bi22;

void ConvertQuaternionToEuler(void) {

	bi00 = q0q0 + q1q1 - q2q2 - q3q3; // yaw
	//bi01 = 2.0f * (q1q2 - q0q3);
	//bi02 = 2.0f * (q0q2 + q1q3);

	bi10 = 2.0f * (q1q2 + q0q3);
	//bi11 = q0q0 - q1q1 + q2q2 - q3q3; // yaw
	//bi12 = 2.0f * (q2q3 - q0q1);

	bi20 = 2.0f * (q1q3 - q0q2); // pitch gx
	bi21 = 2.0f * (q0q1 + q2q3); // roll gy
	bi22 = q0q0 - q1q1 - q2q2 + q3q3; // yaw gz

	A[Roll].Angle = atan2f(bi21, bi22);
	A[Pitch].Angle = -asinf(bi20);
	A[Yaw].Angle = atan2f(bi10, bi00);

} // ConvertQuaternionToEuler


void ConvertEulerToQuaternion(real32 p, real32 r, real32 y) {
	real32 t0, t1, t2, t3, t4, t5, normR;

	t0 = cosf(y * 0.5f);
	t1 = sinf(y * 0.5f);
	t2 = cosf(r * 0.5f);
	t3 = sinf(r * 0.5f);
	t4 = cosf(p * 0.5f);
	t5 = sinf(p * 0.5f);

	q0 = t0 * t2 * t4 + t1 * t3 * t5;
	q1 = t0 * t3 * t4 - t1 * t2 * t5;
	q2 = t0 * t2 * t5 + t1 * t3 * t4;
	q3 = t1 * t2 * t4 - t0 * t3 * t5;

	// Just in case
	normR = invSqrt(Sqr(q0) + Sqr(q1) + Sqr(q2) + Sqr(q3));
	q0 *= normR;
	q1 *= normR;
	q2 *= normR;
	q3 *= normR;

} // ConvertEulerToQuaternion


real32 GravityCompensatedAccZ(void) {

	return 2.0f * (q1q3 - q0q2) * Acc[X] + 2.0f * (q0q1 + q2q3) * Acc[Y]
			+ (q0q0 - q1q1 - q2q2 + q3q3) * Acc[Z] + GRAVITY_MPS_S;
} // GravityCompensatedAccZ

real32 AttitudeCosine(void) { // for attitude throttle compensation

	return q0q0 - q1q1 - q2q2 + q3q3;

} // AttitudeCosine


void VersanoCompensation(void) {
#if defined(USE_VERSANO_GRAVITY_COMP)
	// http://www.varesano.net/blog/fabio/simple-gravity-compensation-9-dom-imus
	// compensate the accelerometer readings from gravity.

	real32 cx, cy, cz;

	// get expected direction of gravity from previous iteration!
	cx = 2.0f * (q1q3 - q0q2);
	cy = 2.0f * (q0q1 + q2q3);
	cz = q0q0 - q1q1 - q2q2 + q3q3;

	ax -= cx;
	ay -= cy;
	az -= cz;

#endif
} // VersanoCompensation


// REVISED MADGWICK


void InitMadgwick(void) {

	GetIMU();

	real32 normR = 1.0f / sqrtf(Sqr(Acc[BF]) + Sqr(Acc[LR]) + Sqr(Acc[UD]));

	A[Pitch].Angle = asinf(-Acc[BF] * normR);
	A[Roll].Angle = asinf(Acc[LR] * normR);

	do {
		GetMagnetometer();
	} while (!F.NewMagValues);
	CalculateMagneticHeading(); // for Madgwick quaternion init

	A[Yaw].Angle = MagHeading;

	ConvertEulerToQuaternion(A[Pitch].Angle, A[Roll].Angle, A[Yaw].Angle);

} // InitMadgwick

void UpdateHeading(void) {

	EstMagHeading += Rate[Yaw] * dT;
	EstMagHeading = Make2Pi(EstMagHeading);

	if (F.Emulation) {
		MagHeading = EstMagHeading;
		Heading = Make2Pi(MagHeading + MagVariation);
	} else if (CurrStateEst == MadgwickIMU) {
		GetMagnetometer();
		if (F.NewMagValues) {
			CalculateMagneticHeading();
			Heading = Make2Pi(MagHeading + MagVariation);
		}
	} else
		Heading = Make2Pi(A[Yaw].Angle + MagVariation);

	MagLockE = MakePi(MagHeading - A[Yaw].Angle);
	F.MagnetometerLocked = Abs(RadiansToDegrees(MagLockE) < 5.0f);

} // UpdateHeading


// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// 19/02/2012	SOH Madgwick Magnetometer measurement is normalised << THIS VERSION
// corrected by J. van de Mortel 2014
// modified by Prof. G.K. Egan 2017

#define gyroMeasError DegreesToRadians(0.5f) // gyroscope measurement error in rad/s (shown as 5 deg/s)
#define gyroMeasDrift DegreesToRadians(0.02f) // gyroscope measurement error in rad/s/s (shown as 0.2f deg/s/s)
#define beta 0.2f
#define beta1 (sqrt(0.75f) * gyroMeasError) // compute beta
#define zeta (sqrt(0.75f) * gyroMeasDrift) // compute zeta
// paper suggest Beta=0.041 for MARG
//#define betaDef 0.1f // 2 * proportional gain
//real32 beta = betaDef; // 2 * proportional gain (Kp)
real32 BetaBase;

void MadgwickMARGUpdate(real32 gx, real32 gy, real32 gz, real32 ax, real32 ay,
		real32 az, real32 mx, real32 my, real32 mz) {
	real32 Beta;
	real32 AccMag, normR;
	real32 s0, s1, s2, s3;

	real32 hx, hy;
	real32 wq0, wq1, wq2, wq3;
	real32 _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _8bx, _8bz,
			_2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2;

	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;

	_2q0 = 2.0f * q0;
	_2q1 = 2.0f * q1;
	_2q2 = 2.0f * q2;
	_2q3 = 2.0f * q3;

	// Rate of change of quaternion from gyroscope
	wq0 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	wq1 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	wq2 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	wq3 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	AccMag = sqrtf(Sqr(ax) + Sqr(ay) + Sqr(az));
	AccConfidence = CalculateAccConfidence(AccMag);

	// Normalise accelerometer measurement
	normR = invSqrt(AccMag);
	ax *= normR;
	ay *= normR;
	az *= normR;

	// default KpAccBase is 0.2 paper says 0.041
	Beta = State == InFlight ? BetaBase * AccConfidence : BetaBase * 2.5f;

	GetMagnetometer();

	if (F.NewMagValues) {
		F.NewMagValues = false;

		CalculateMagneticHeading(); // for lock reference

		// Normalise magnetometer measurement
		normR = invSqrt(Sqr(mx) + Sqr(my) + Sqr(mz));
		mx *= normR;
		my *= normR;
		mz *= normR;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0f * q0 * mx;
		_2q0my = 2.0f * q0 * my;
		_2q0mz = 2.0f * q0 * mz;
		_2q1mx = 2.0f * q1 * mx;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2
				+ _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1
				+ my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1
				+ _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;
		_8bx = 2.0f * _4bx;
		_8bz = 2.0f * _4bz;

		// Gradient descent algorithm corrective step
		s0 = -_2q2 * (2.0f * (q1q3 - q0q2) - ax) + _2q1 * (2.0f * (q0q1 + q2q3)
				- ay) + -_4bz * q2 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3
				- q0q2) - mx) + (-_4bx * q3 + _4bz * q1) * (_4bx
				* (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + _4bx * q2
				* (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * (q1q3 - q0q2) - ax) + _2q0 * (2.0f * (q0q1 + q2q3)
				- ay) + -4.0f * q1 * (2.0f * (0.5 - q1q1 - q2q2) - az) + _4bz
				* q3 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx)
				+ (_4bx * q2 + _4bz * q0) * (_4bx * (q1q2 - q0q3) + _4bz
						* (q0q1 + q2q3) - my) + (_4bx * q3 - _8bz * q1) * (_4bx
				* (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * (q1q3 - q0q2) - ax) + _2q3 * (2.0f * (q0q1 + q2q3)
				- ay) + (-4.0f * q2) * (2.0f * (0.5 - q1q1 - q2q2) - az)
				+ (-_8bx * q2 - _4bz * q0) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz
						* (q1q3 - q0q2) - mx) + (_4bx * q1 + _4bz * q3) * (_4bx
				* (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * q0
				- _8bz * q2) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1
				- q2q2) - mz);
		s3 = _2q1 * (2.0f * (q1q3 - q0q2) - ax) + _2q2 * (2.0f * (q0q1 + q2q3)
				- ay) + (-_8bx * q3 + _4bz * q1) * (_4bx * (0.5 - q2q2 - q3q3)
				+ _4bz * (q1q3 - q0q2) - mx) + (-_4bx * q0 + _4bz * q2) * (_4bx
				* (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * q1)
				* (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
	} else {

		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;

		// Gradient descent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1
				+ _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2
				+ _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
	}

	// normalise step magnitude
	normR = invSqrt(Sqr(s0) + Sqr(s1) + Sqr(s2) + Sqr(s3));
	s0 *= normR;
	s1 *= normR;
	s2 *= normR;
	s3 *= normR;

	// Apply feedback step
	wq0 -= Beta * s0;
	wq1 -= Beta * s1;
	wq2 -= Beta * s2;
	wq3 -= Beta * s3;

	// Integrate rate of change of quaternion to yield quaternion
	q0 += wq0 * dT;
	q1 += wq1 * dT;
	q2 += wq2 * dT;
	q3 += wq3 * dT;

	// Normalise quaternion
	normR = invSqrt(Sqr(q0) + Sqr(q1) + Sqr(q2) + Sqr(q3));
	q0 *= normR;
	q1 *= normR;
	q2 *= normR;
	q3 *= normR;

	ConvertQuaternionToEuler();

	A[Yaw].Angle = -A[Yaw].Angle;
	A[Pitch].Angle = -A[Pitch].Angle;
	if (A[Roll].Angle < 0)
		A[Roll].Angle += PI;
	else
		A[Roll].Angle -= PI;

} // MadgwickMARGUpdate


//___________________________________________________________________________

// Madgwick Quaternion version of Mahony et al.  Discrete Cosine Transform code
// rewritten by Prof G.K. Egan

void MadgwickUpdate(boolean AHRS, real32 gx, real32 gy, real32 gz, real32 ax,
		real32 ay, real32 az, real32 mx, real32 my, real32 mz) {

	real32 normR;

	real32 q0i, q1i, q2i, q3i;
	real32 hx, hy, hz;
	real32 bx, bz;
	real32 vx, vy, vz;
	real32 wx, wy, wz;

	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;

	// estimated direction of gravity (v)
	vx = 2.0f * (q1q3 - q0q2);
	vy = 2.0f * (q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;

	// error is sum of cross product between reference direction
	// of fields and direction measured by sensors

	real32 AccMag = sqrtf(Sqr(ax) + Sqr(ay) + Sqr(az));
	AccConfidence = CalculateAccConfidence(AccMag);

	// renormalise to attempt to remove a little acc vibration noise
	normR = 1.0f / AccMag;
	ax *= normR;
	ay *= normR;
	az *= normR;

	KpAcc = State == InFlight ? KpAccBase * AccConfidence : KpAccBase * 5.0f;
	//KpAcc = KpAccBase * AccConfidence;
	gx += (vy * az - vz * ay) * KpAcc;
	gy += (vz * ax - vx * az) * KpAcc;
	gz += (vx * ay - vy * ax) * KpAcc;

	if (AHRS) {

		GetMagnetometer();

		if (F.NewMagValues) { // no compensation for latency
			F.NewMagValues = false;

			CalculateMagneticHeading(); // for lock reference

			KpMag = CalculateMagConfidence();

			normR = invSqrt(Sqr(mx) + Sqr(my) + Sqr(mz));
			mx *= normR;
			my *= normR;
			mz *= normR;

			// reference direction of flux
			hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz
					* (q1q3 + q0q2));
			hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz
					* (q2q3 - q0q1));
			hz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f
					- q1q1 - q2q2));

			bx = sqrtf(Sqr(hx) + Sqr(hy));
			bz = hz;

			// estimated direction of flux (w)
			wx = 2.0f * (bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2));
			wy = 2.0f * (bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3));
			wz = 2.0f * (bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2));

			gx += (my * wz - mz * wy) * KpMag;
			gy += (mz * wx - mx * wz) * KpMag;
			gz += (mx * wy - my * wx) * KpMag;
		}
	}

	// integrate quaternion rate
	q0i = (-q1 * gx - q2 * gy - q3 * gz) * dTOn2;
	q1i = (q0 * gx + q2 * gz - q3 * gy) * dTOn2;
	q2i = (q0 * gy - q1 * gz + q3 * gx) * dTOn2;
	q3i = (q0 * gz + q1 * gy - q2 * gx) * dTOn2;
	// two steps to preserve old to new q
	q0 += q0i;
	q1 += q1i;
	q2 += q2i;
	q3 += q3i;

	// normalize quaternion
	normR = invSqrt(Sqr(q0) + Sqr(q1) + Sqr(q2) + Sqr(q3));
	q0 *= normR;
	q1 *= normR;
	q2 *= normR;
	q3 *= normR;

	ConvertQuaternionToEuler();

} // MadgwickUpdate


void TrackPitchAttitude(void) {
	static uint32 GlidingTimemS = 0;
	static real32 PitchAngle = 0.0f;

	if (IsFixedWing && (DesiredThrottle < IdleThrottle) && (State == InFlight)) {
		if (mSClock() > GlidingTimemS) {
			PitchAngle
					= HardFilter(PitchAngle, RadiansToDegrees(A[Pitch].Angle));
			SetP(FWGlideAngleOffset, (uint8)(PitchAngle * 10.0f)); // *(int8 *)
		}
	} else
		GlidingTimemS = mSClock() + 5000;
} // TrackPitchAttitude


void UpdateInertial(void) {
	int32 a;

	if (F.Emulation)
		DoEmulation(); // produces ROC, Altitude etc.
	else
		GetIMU();

	if (CurrStateEst == MadgwickMARG)
		MadgwickMARGUpdate(Rate[Roll], Rate[Pitch], Rate[Yaw], Acc[BF],
				Acc[LR], Acc[UD], Mag[X], Mag[Y], Mag[Z]);
	else
		MadgwickUpdate(CurrStateEst == MadgwickAHRS, Rate[Roll], Rate[Pitch],
				Rate[Yaw], Acc[BF], Acc[LR], Acc[UD], Mag[X], Mag[Y], Mag[Z]);

	DoControl();

	// one cycle delay OK
	UpdateHeading();

	UpdateGPS();
	if (F.NewGPSPosition) {
		F.NewGPSPosition = false;
		for (a = NorthC; a <= DownC; a++) {
			Nav.Pos[a] = GPS.Pos[a];
			Nav.Vel[a] = GPS.Vel[a];
		}
		CheckNavEnable();
	}

	if (!F.Emulation)
		UpdateAltitudeEstimates();

	if (!IsMulticopter)
		UpdateAirspeed();

	TrackPitchAttitude();

}// UpdateInertial


//____________________________________________________________________________


void CheckNavEnable(void) {

	if ((mSClock() > mS[NavActiveTime]) && !F.NavigationActive) {
		F.NavigationActive = true;
		ResumeHoldingStation();
		NavSwStateP = NavSwUnknown;
	}

	//	if (!F.NavigationEnabled)
	//		AcquireHoldPosition();

	F.GPSPosUpdated = true; // for telemetry
	F.NewNavUpdate = F.NavigationEnabled;

} // CheckNavEnable


