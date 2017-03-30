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

int32 yymmdd_to_julian_days(int16 yy, int16 mm, int16 dd) {
	int32 jd;

	jd = dd - 32075L + 1461L * (yy + 4800L + (mm - 14) / 12) / 4;
	jd = jd + 367L * (mm - 2 - (mm - 14) / 12 * 12) / 12;
	jd = jd - 3 * ((yy + 4900L + (mm - 14) / 12) / 100) / 4;

	return (jd);
} // yymmdd_to_julian_days


const real32 testresult[] = { -5.54, 6.7, 16.53, 11.29, 69.87 };

const real32 testlat[] = { 0, 30, -30, -37, -80 };
const real32 testlon[] = { 0, 180, -180, 145, -120 };

real32 ComputeMagVar(void) {
	real32 field[6];
	real32 d;
	int32 dat;

#if defined(TEST_MAGVAR)

	uint8 i, a;

	dat = yymmdd_to_julian_days(2014, 3, 18);

	TxString(0, "Model, Lat, Lon, B_r, B_theta,B_phi,X,Y,Z,Known, Result\n");

	for (i = 0; i < 5; i++) {
		d = SGMagVar(DegreesToRadians(testlat[i]),
				DegreesToRadians(testlon[i]), 0.0f, dat, field);
		TxVal32(0, testlat[i], 0, ',');
		TxVal32(0, testlon[i], 0, ',');
		for (a = 0; a < 6; a++)
			TxVal32(0, field[a], 0, ',');
		TxVal32(0, testresult[i] * 10.0f, 1, ',');
		TxVal32(0, RadiansToDegrees(d) * 10.0f, 1, 0);
		TxNextLine(0);
	}

#else

	dat = yymmdd_to_julian_days((int16)GPS.year, (int16)GPS.month, (int16)GPS.day);

	d = SGMagVar(DegreesToRadians((real64)GPS.lat * 1e-7),
			DegreesToRadians((real64)GPS.lon * 1e-7), 0.0f, dat,
			field);

#endif
	return (d);

} // ComputeMagVar


real32 SGMagVar(real64 lat, real64 lon, real64 h, int32 dat,
		real32 * field) {

	/*  module magfield.c */
	// http://williams.best.vwh.net/magvar/magfield.c

	/* Module to calculate magnetic variation and field given position,
	 **               altitude, and date
	 ** Implements the NIMA (formerly DMA) WMM and IGRF models
	 **
	 **    http://www.nima.mil/GandG/ngdc-wmm2000.html
	 **    For WMM2000 coefficients:
	 **    ftp://ftp.ngdc.noaa.gov/Solid_Earth/Mainfld_Mag/DoD_Model/wmm.cof
	 **    For IGRF/DGRF coefficients:
	 **    http://swdcdb.kugi.kyoto-u.ac.jp/igrf/coef/igrfall.d
	 **
	 ** Copyright (C) 2000  Edward A Williams <Ed_Williams@compuserve.com>
	 ** C# Port by Michael Coyle <Michael.Coyle@BlueToque.ca>,
	 **
	 **  The routine uses a spherical harmonic expansion of the magnetic
	 ** potential up to twelfth order, together with its time variation, as
	 ** described in Chapter 4 of "Geomagnetism, Vol 1, Ed. J.A.Jacobs,
	 ** Academic Press (London 1987)". The program first converts geodetic
	 ** coordinates (lat/long on elliptic earth and altitude) to spherical
	 ** geocentric (spherical lat/long and radius) coordinates. Using this,
	 ** the spherical (B_r, B_theta, B_phi) magnetic field components are
	 ** computed from the model. These are finally referred to surface (X, Y,
	 ** Z) coordinates.
	 **
	 **   Fields are accurate to better than 200nT, variation and dip to
	 ** better than 0.5 degrees, with the exception of the declination near
	 ** the magnetic poles (where it is ill-defined) where the error may reach
	 ** 4 degrees or more.
	 **
	 **   Variation is undefined at both the geographic and
	 ** magnetic poles, even though the field itself is well-behaved. To
	 ** avoid the routine blowing up, latitude entries corresponding to
	 ** the geographic poles are slightly offset. At the magnetic poles,
	 ** the routine returns zero variation.
	 **
	 ** HISTORY
	 ** Adapted from EAW Excel 3.0 version 3/27/94 EAW
	 ** Recoded in C++ by Starry Chan
	 ** WMM95 added and rearranged in ANSI-C EAW 7/9/95
	 ** Put shell around program and made Borland & GCC compatible EAW 11/22/95
	 ** IGRF95 added 2/96 EAW
	 ** WMM2000 IGR2000 added 2/00 EAW
	 ** Released under GPL  3/26/00 EAW
	 ** Adaptions and modifications for the SimGear project  3/27/2000 CLO
	 ** Removed all pow() calls and made static roots arrays to
	 ** save many sqrt() calls on subsequent invocations
	 ** 3/28/2000  Norman Vine -- nhv@yahoo.com
	 ** Put in some bullet-proofing to handle magnetic and geographic poles.
	 ** 3/28/2000 EAW
	 ** Added missing comment close, the lack of which caused the altitude
	 ** correction to be omitted.
	 ** 01/31/01 Jim Seymour (jseymour@LinxNet.com)
	 ** 23/01/13 POrt to C# (michael.coyle@BlueToque.ca)
	 ** 10/12/14 Adapted for UAVX by Prof G.K. Egan
	 **
	 ** This program is free software; you can redistribute it and/or
	 ** modify it under the terms of the GNU General Public Licence as
	 ** published by the Free Software Foundation; either version 2 of the
	 ** Licence, or (at your option) any later version.
	 **
	 ** This program is distributed in the hope that it will be useful, but
	 ** WITHOUT ANY WARRANTY; without even the implied warranty of
	 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	 ** General Public Licence for more details.
	 **
	 ** You should have received a copy of the GNU General Public Licence
	 ** along with this program; if not, write to the Free Software
	 ** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	 **
	 */

	const double a = 6378.16; /* major radius (km) IAU66 ellipsoid */
	const double b = 6378.16 * (1.0 - 1.0 / 298.25);
	const double r_0 = 6371.2; /* "mean radius" for spherical harmonic expansion */

	const double gnm_wmm2010[13][13] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0 }, { -29496.6, -1586.3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
			-2396.6, 3026.1, 1668.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 1340.1,
			-2326.2, 1231.9, 634, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 912.6, 808.9,
			166.7, -357.1, 89.4, 0, 0, 0, 0, 0, 0, 0, 0 }, { -230.9, 357.2,
			200.3, -141.1, -163, -7.8, 0, 0, 0, 0, 0, 0, 0 }, { 72.8, 68.6, 76,
			-141.4, -22.8, 13.2, -77.9, 0, 0, 0, 0, 0, 0 }, { 80.5, -75.1,
			-4.7, 45.3, 13.9, 10.4, 1.7, 4.9, 0, 0, 0, 0, 0 }, { 24.4, 8.1,
			-14.5, -5.6, -19.3, 11.5, 10.9, -14.1, -3.7, 0, 0, 0, 0 }, { 5.4,
			9.4, 3.4, -5.2, 3.1, -12.4, -0.7, 8.4, -8.5, -10.1, 0, 0, 0 }, {
			-2, -6.3, 0.9, -1.1, -0.2, 2.5, -0.3, 2.2, 3.1, -1, -2.8, 0, 0 }, {
			3, -1.5, -2.1, 1.7, -0.5, 0.5, -0.8, 0.4, 1.8, 0.1, 0.7, 3.8, 0 },
			{ -2.2, -0.2, 0.3, 1, -0.6, 0.9, -0.1, 0.5, -0.4, -0.4, 0.2, -0.8,
					0 } };

	const double hnm_wmm2010[13][13] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0 }, { 0, 4944.4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, -2707.7,
			-576.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, -160.2, 251.9, -536.6,
			0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 286.4, -211.2, 164.3, -309.1, 0,
			0, 0, 0, 0, 0, 0, 0 }, { 0, 44.6, 188.9, -118.2, 0, 100.9, 0, 0, 0,
			0, 0, 0, 0 }, { 0, -20.8, 44.1, 61.5, -66.3, 3.1, 55, 0, 0, 0, 0,
			0, 0 },
			{ 0, -57.9, -21.1, 6.5, 24.9, 7, -27.7, -3.3, 0, 0, 0, 0, 0 }, { 0,
					11, -20, 11.9, -17.4, 16.7, 7, -10.8, 1.7, 0, 0, 0, 0 },
			{ 0, -20.5, 11.5, 12.8, -7.2, -7.4, 8, 2.1, -6.1, 7, 0, 0, 0 },
			{ 0, 2.8, -0.1, 4.7, 4.4, -7.2, -1, -3.9, -2, -2, -8.3, 0, 0 }, {
					0, 0.2, 1.7, -0.6, -1.8, 0.9, -0.4, -2.5, -1.3, -2.1, -1.9,
					-1.8, 0 }, { 0, -0.9, 0.3, 2.1, -2.5, 0.5, 0.6, 0, 0.1,
					0.3, -0.9, -0.2, 0.9 } };

	const double gtnm_wmm2010[13][13] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0 }, { 11.6, 16.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -12.1,
			-4.4, 1.9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.4, -4.1, -2.9, -7.7,
			0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1.8, 2.3, -8.7, 4.6, -2.1, 0, 0, 0,
			0, 0, 0, 0, 0 },
			{ -1, 0.6, -1.8, -1, 0.9, 1, 0, 0, 0, 0, 0, 0, 0 }, { -0.2, -0.2,
					-0.1, 2, -1.7, -0.3, 1.7, 0, 0, 0, 0, 0, 0 }, { 0.1, -0.1,
					-0.6, 1.3, 0.4, 0.3, -0.7, 0.6, 0, 0, 0, 0, 0 }, { -0.1,
					0.1, -0.6, 0.2, -0.2, 0.3, 0.3, -0.6, 0.2, 0, 0, 0, 0 },
			{ 0, -0.1, 0, 0.3, -0.4, -0.3, 0.1, -0.1, -0.4, -0.2, 0, 0, 0 },
			{ 0, 0, -0.1, 0.2, 0, -0.1, -0.2, 0, -0.1, -0.2, -0.2, 0, 0 }, { 0,
					0, 0, 0.1, 0, 0, 0, 0, 0, 0, -0.1, 0, 0 }, { 0, 0, 0.1,
					0.1, -0.1, 0, 0, 0, 0, 0, 0, -0.1, 0.1 } };

	const double htnm_wmm2010[13][13] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0 }, { 0, -25.9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, -22.5,
			-11.8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 7.3, -3.9, -2.6, 0, 0,
			0, 0, 0, 0, 0, 0, 0 }, { 0, 1.1, 2.7, 3.9, -0.8, 0, 0, 0, 0, 0, 0,
			0, 0 }, { 0, 0.4, 1.8, 1.2, 4, -0.6, 0, 0, 0, 0, 0, 0, 0 }, { 0,
			-0.2, -2.1, -0.4, -0.6, 0.5, 0.9, 0, 0, 0, 0, 0, 0 }, { 0, 0.7,
			0.3, -0.1, -0.1, -0.8, -0.3, 0.3, 0, 0, 0, 0, 0 }, { 0, -0.1, 0.2,
			0.4, 0.4, 0.1, -0.1, 0.4, 0.3, 0, 0, 0, 0 }, { 0, 0, -0.2, 0, -0.1,
			0.1, 0, -0.2, 0.3, 0.2, 0, 0, 0 }, { 0, 0.1, -0.1, 0, -0.1, -0.1,
			0, -0.1, -0.2, 0, -0.1, 0, 0 }, { 0, 0, 0.1, 0, 0.1, 0, 0.1, 0,
			-0.1, -0.1, 0, -0.1, 0 },
			{ 0, 0, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0, 0 } };

	const int nmax = 12;

	double P[13][13];
	double DP[13][13];
	double gnm[13][13];
	double hnm[13][13];
	double sm[13];
	double cm[13];

	static double root[13];
	static double roots[13][13][2];

	static boolean Primed = false;

	/*
	 * return variation (in radians) given geodetic latitude (radians), longitude
	 * (radians) ,height (km) and (Julian) date
	 * model=1 is IGRF90, 2 is WMM85, 3 is WMM90, 4 is WMM95,
	 * 5 is IGRF95, 6 is WMM2000, 7 is IGRF2000, 8 is WMM2005, 9 is IGRF2005, 10 is WMM2010, 11 is IGRF2010
	 * N and E lat and long are positive, S and W negative
	 */
	/// <summary>
	///
	/// </summary>
	/// <param name="lat">latitude in radians</param>
	/// <param name="lon">longitude in radians</param>
	/// <param name="h">altitude in km</param>
	/// <param name="dat">julian date</param>
	/// <param name="model">model to use:1 is IGRF90, 2 is WMM85, 3 is WMM90, 4 is WMM95 </param>
	/// <param name="field"></param>
	/// <returns>Magnetic variation in radians</returns>

	/* output field B_r,B_th,B_phi,B_x,B_y,B_z */
	int n, m;
	double yearfrac, sr, r, theta, c, s, psi, fn, fn_0, B_r, B_theta, B_phi, X,
			Y, Z;
	double sinpsi, cospsi, inv_s;

	//static int Primed = 0;

	double sinlat = sin(lat);
	double coslat = cos(lat);

	/* convert to geocentric */
	sr = sqrt(a * a * coslat * coslat + b * b * sinlat * sinlat);
	/* sr is effective radius */
	theta = atan2(coslat * (h * sr + a * a), sinlat * (h * sr + b * b));

	/* theta is geocentric co-latitude */

	r = h * h + 2.0 * h * sr + (a * a * a * a - (a * a * a * a - b * b * b * b)
			* sinlat * sinlat) / (a * a - (a * a - b * b) * sinlat * sinlat);

	r = sqrt(r);

	/* r is geocentric radial distance */
	c = cosf(theta);
	s = sinf(theta);
	/* protect against zero divide at geographic poles */
	//inv_s =  1.0 / (s +(s == 0.0)*1.0e-8);
	inv_s = (s == 0) ? 1.0 / (s + 1 * 1.0e-8) : 1.0 / (s + 0 * 1.0e-8);

	/*zero out arrays */
	for (n = 0; n <= nmax; n++) {
		for (m = 0; m <= n; m++) {
			P[n][m] = DP[n][m] = 0;
		}
	}

	/* diagonal elements */
	P[0][0] = 1;
	P[1][1] = s;
	DP[0][0] = 0;
	DP[1][1] = c;
	P[1][0] = c;
	DP[1][0] = -s;

	/* these values will not change for subsequent function calls */
	if (!Primed) {
		for (n = 2; n <= nmax; n++)
			root[n] = sqrt((2.0 * n - 1) / (2.0 * n));

		for (m = 0; m <= nmax; m++) {
			double mm = m * m;
			for (n = Max(m + 1, 2); n <= nmax; n++) {
				roots[m][n][0] = sqrt((n - 1) * (n - 1) - mm);
				roots[m][n][1] = 1.0 / sqrt(n * n - mm);
			}
		}
		Primed = true;
	}

	for (n = 2; n <= nmax; n++) {
		/*  double root = sqrt((2.0*n-1) / (2.0*n)); */
		P[n][n] = P[n - 1][n - 1] * s * root[n];
		DP[n][n] = (DP[n - 1][n - 1] * s + P[n - 1][n - 1] * c) * root[n];
	}

	/* lower triangle */
	for (m = 0; m <= nmax; m++) {
		/*  double mm = m*m;  */
		for (n = Max(m + 1, 2); n <= nmax; n++) {
			/* double root1 = sqrt((n-1)*(n-1) - mm); */
			/* double root2 = 1.0 / sqrt( n*n - mm);  */
			P[n][m] = (P[n - 1][m] * c * (2.0f * n - 1) - P[n - 2][m]
					* roots[m][n][0]) * roots[m][n][1];
			DP[n][m] = ((DP[n - 1][m] * c - P[n - 1][m] * s) * (2.0f * n - 1)
					- DP[n - 2][m] * roots[m][n][0]) * roots[m][n][1];
		}
	}

	/* compute gnm, hnm at dat */

	yearfrac = (dat - yymmdd_to_julian_days(2010, 1, 1)) / 365.25;
	for (n = 1; n <= nmax; n++)
		for (m = 0; m <= nmax; m++) {
			gnm[n][m] = gnm_wmm2010[n][m] + yearfrac * gtnm_wmm2010[n][m];
			hnm[n][m] = hnm_wmm2010[n][m] + yearfrac * htnm_wmm2010[n][m];
		}

	/* compute sm (sin(m lon) and cm (cos(m lon)) */
	for (m = 0; m <= nmax; m++) {
		sm[m] = sin(m * lon);
		cm[m] = cos(m * lon);
	}

	/* compute B fields */
	B_r = 0.0;
	B_theta = 0.0;
	B_phi = 0.0;
	fn_0 = r_0 / r;
	fn = fn_0 * fn_0;

	for (n = 1; n <= nmax; n++) {
		double c1_n = 0;
		double c2_n = 0;
		double c3_n = 0;
		for (m = 0; m <= n; m++) {
			double tmp = (gnm[n][m] * cm[m] + hnm[n][m] * sm[m]);
			c1_n += tmp * P[n][m];
			c2_n += tmp * DP[n][m];
			c3_n += m * (gnm[n][m] * sm[m] - hnm[n][m] * cm[m]) * P[n][m];
		}
		/* fn=pow(r_0/r,n+2.0);   */
		fn *= fn_0;
		B_r += (n + 1) * c1_n * fn;
		B_theta -= c2_n * fn;
		B_phi += c3_n * fn * inv_s;
	}

	/* Find geodetic field components: */
	psi = theta - (PI / 2.0 - lat);
	sinpsi = sin(psi);
	cospsi = cos(psi);
	X = -B_theta * cospsi - B_r * sinpsi;
	Y = B_phi;
	Z = B_theta * sinpsi - B_r * cospsi;

	field[0] = B_r;
	field[1] = B_theta;
	field[2] = B_phi;
	field[3] = X;
	field[4] = Y;
	field[5] = Z; /* output fields */
	/* find variation in radians */
	/* return zero variation at magnetic pole X=Y=0. */
	/* E is positive */
	return (X != 0.0 || Y != 0.0) ? atan2(Y, X) : (double) 0.0;

} // SGMagVar


