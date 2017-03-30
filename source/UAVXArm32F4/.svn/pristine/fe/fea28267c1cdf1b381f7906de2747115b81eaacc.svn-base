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


#ifndef _stats_h
#define _stats_h

#define INIT_MIN 1000L

void ZeroStats(void);
void ReadStatsNV(void);
void WriteStatsNV(void);
void ShowStats(uint8 s);
void StatsMinMax(int16 v, uint8 l, uint8 u);
void StatsMax(int16 v, uint8 u);

enum Statistics {
	GPSAltitudeS,
	BaroAltitudeS,
	ESCI2CFailS,
	GPSMinSatsS,
	MinROCS,
	MaxROCS,
	GPSVelS,
	AccFailS,
	CompassFailS,
	BaroFailS,

	GPSInvalidS,
	GPSMaxSatsS,
	OriginValidS,
	MinhAccS,
	MaxhAccS,
	RCGlitchesS,
	SPIFailS,
	GyroFailS,
	RCFailsafesS,
	I2CFailS,

	UtilisationS,
	BadS,
	BadNumS
};
// NO MORE THAN 32 or 64 bytes

#endif


