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


#ifndef _mission_h
#define _mission_h

#define NAV_MAX_WAYPOINTS 11

typedef struct {
	int32 LatitudeRaw; // 1e7/degree
	int32 LongitudeRaw;
	int16 Altitude;
	int16 VelocitydMpS;
	int16 Loiter;
	int16 OrbitRadius;
	int16 OrbitAltitude; // relative to Origin
	int16 OrbitVelocitydMpS;
	uint8 Action;
}__attribute__((packed)) WPStructNV;

typedef struct {
	int8 NoOfWayPoints;
	int8 ProximityAltitude;
	int8 ProximityRadius;
	int16 OriginAltitude;
	int32 OriginLatitude;
	int32 OriginLongitude;
	int16 RTHAltHold; // ??? not used
	WPStructNV WP[NAV_MAX_WAYPOINTS];
}__attribute__((packed)) MissionStruct;

extern void GenerateNavTestMission(void);
extern void DisplayNavMissions(uint8 s);
extern boolean NavMissionSanityCheck(MissionStruct * M);
extern uint8 NextWPState(void);
extern void RefreshNavWayPoint(void);
extern void GetNavWayPoint(void);
extern void DisplayNavMission(uint8 s, MissionStruct * M);
extern void DoMissionUpdate(void);
extern void ClearNavMissions(uint8 s);

extern WPStruct WP, HP, POI;
extern uint8 CurrWPNo, PrevWPNo;

#endif


