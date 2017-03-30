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


#ifndef _altitude_h
#define _altitude_h

real32 Airspeed; // zzz logical place for air related stuff

//#define MS56XX_CYCLE_MS 	10
//#define MS56XX_HZ			(1000.0f/MS56XX_CYCLE_MS)


typedef struct {
	uint8 Samples;
	uint16 UpdateuS;
	uint32 OSSMask;
} ms56xxDefStruct;

enum BaroTypes {
	BMP085Baro, MS5611Baro, MS5607Baro, BaroUnknown
};

typedef const struct {
	uint16 intervalmS;
	real32 minAlt;
	real32 maxAlt;
} RFStruct;

extern uint8 CurrRFSensorType;

extern RFStruct RF[];

// Measurement Specialities Baro

//#define MS56XX_ID 0xee
#define MS56XX_ID (0x77*2)

void ReadBaroCalibration(void);
real32 CompensateBaro(uint32 ut, uint32 up);
real32 CompensateBaro2(uint32 ut, uint32 up);
void StartBaro(boolean ReadPressure);
boolean BaroCheckCRC(void);
boolean IsBaroActive(void);
void GetBaro(void);
void GetDensityAltitude(void);
void InitBarometer(void);

#define MAXSONAR_ID 0xe0
#define SRFSONAR_ID 0xe0

enum RangeFinders {
	MaxSonarcm, SRFI2Ccm, MaxSonarI2Ccm, SharpIRGP2Y0A02YK, SharpIRGP2Y0A710K, UnknownRF
};

extern const char * TelemetryName[];
extern void GetRangefinderAltitude(void);
extern void InitRangefinder(void);

extern real32 RangefinderAltitude, RangefinderROC;

void UpdateAltitudeEstimates(void);

extern uint16 ms56xx_c[];

void UpdateAltitudeAndROC(void);
void BaroTest(uint8 s);

extern real32 BaroPressure, BaroTemperature, UncompensatedBaroPressure, UncompensatedBaroAltitude;
extern boolean AcquiringPressure;
extern real32 OriginAltitude, BaroAltitude;
extern real32 ROC, ROCF;
extern int32 BaroVal;
extern uint8 BaroType;
extern real32 AltdT;
extern real32 AltLPFHz;
extern uint16 ms56xx_ManufacturersData;
extern real32 FAltitude, FROC; // fusion filter output

#endif

