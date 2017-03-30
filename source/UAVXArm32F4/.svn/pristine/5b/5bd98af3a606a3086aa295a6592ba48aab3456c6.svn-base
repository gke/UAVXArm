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


void ZeroStats(void) {
	uint16 s;
	int32 a;

	for (s = 0; s < MAX_STATS; s++)
		NV.Stats[s] = 0;

	NV.Stats[MinhAccS] = INIT_MIN;
	NV.Stats[MaxhAccS] = 0;
	NV.Stats[MinROCS] = INIT_MIN;
	NV.Stats[MaxROCS] = 0;
	NV.Stats[GPSMinSatsS] = INIT_MIN;
	NV.Stats[GPSMaxSatsS] = 0;

	for (s = 0; s < MAG_MAX_HIST; s++)
		for (a = 0; a < 4; a++)
			NV.MagCal.Stats[s][a] = 0;
	MagSample = 0;

} // ZeroStats

void StatsMinMax(int16 v, uint8 l, uint8 u) {

	if (v > NV.Stats[u])
		NV.Stats[u] = v;
	else if (v < NV.Stats[l])
		NV.Stats[l] = v;

} // StatsMaxMin

void StatsMax(int16 v, uint8 u) {

	if (v > NV.Stats[u])
		NV.Stats[u] = v;

} // StatsMax

void WriteStatsNV() {
	//uint8 i;

	//if (CurrESCType != ESCPWM)
	//	for (i = 0; i < CurrMaxPWMOutputs; i++)
	//		NV.Stats[ESCI2CFailS] += ESCI2CFail[i];

	UpdateNV();

} // WriteStatsNV



