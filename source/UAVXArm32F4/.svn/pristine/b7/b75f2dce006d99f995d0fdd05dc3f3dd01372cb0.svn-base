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

// 	GPS routines

#include "UAVX.h"

// Moving average of coordinates needed - or Kalman Estimator probably

#define GPSVelocityFilter NoFilter		// done after position filter
const uint8 NMEATags[MAX_NMEA_SENTENCES][5] = {
// NMEA
		{ 'G', 'P', 'G', 'G', 'A' }, // position fix
		{ 'G', 'P', 'R', 'M', 'C' }, // ground speed and heading
		{ 'G', 'N', 'G', 'G', 'A' }, // position fix
		{ 'G', 'N', 'R', 'M', 'C' }, // ground speed and heading
		};

uint8 CurrGPSType;
uint8 CurrGyroLPF;

uint8 RxState = WaitSentinel;

NMEAStruct NMEA;

uint8 GPSPacketTag;

GPSRec GPS;
const uint32 GPSBaud = 115200;
const uint32 UBXGPSBaud = 115200; //230400;

real32 GPSdT, GPSdTR;
real32 GPSLag = 1.0f; // MTK 0.5 for UBlox

uint32 LastGPSUpdatemS = 0;

uint8 nll, cc, lo, hi, ll, ss, tt, GPSCheckSumChar;
uint8 GPSTxCheckSum, RxCheckSum;
boolean EmptyField;
int16 ValidGPSSentences;

#define DEFAULT_BAUD_RATES 7
const uint32 DefaultBaud[] =
		{ 4800, 9600, 19200, 38400, 57600, 115200, 230400 };

void TxGPSString(uint8 s, const char *pch) {
	while (*pch != (char) 0) {
		TxChar(s, *pch++);
		Delay1mS(5);
	}
} // TxGPSString

//______________________________________________________________________________

// UBlox Code rewritten from AQ (C) Bill Nesbitt

uint16 UbxVersion;

const uint8 ShortNMEADisableEnable[] = { 0xB5, 0x62, 0x06, 0x00, 0x14, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00,
		0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBE, 0x72, 0xB5, 0x62,
		0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22 };

const uint8 DISABLE_NMEA[] = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0xF0,
		0x05,
		0x00,
		0xFF,
		0x19, //disable all default NMEA messages
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17 };

const uint8 DISABLE_UBX[] = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x30,
		0x00, 0x45, 0xC0, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x50, 0x00,
		0x65, 0x00, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x33, 0x00, 0x48,
		0xC6, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x31, 0x00, 0x46, 0xC2,
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0B, 0x00, 0x00, 0x15, 0x60,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x00, 0x1C, 0x73, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x00, 0x1C, 0x73, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x10, 0x02, 0x00, 0x1C, 0x73, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x10, 0x10, 0x00, 0x2A, 0x8F,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x0E, 0x00, 0x39, 0xBE, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x21, 0x08, 0x00, 0x33, 0xB2, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x21, 0x09, 0x00, 0x34, 0xB4, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x21, 0x0B, 0x00, 0x36, 0xB8, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x21, 0x0F, 0x00, 0x3A, 0xC0, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x21, 0x0D, 0x00, 0x38, 0xBC,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x05, 0x00, 0x19, 0x67, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x09, 0x00, 0x1D, 0x6F, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x0A, 0x0B, 0x00, 0x1F, 0x73, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x0A, 0x02, 0x00, 0x16, 0x61, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x0A, 0x06, 0x00, 0x1A, 0x69, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x0A, 0x07, 0x00, 0x1B, 0x6B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0x0A, 0x21, 0x00, 0x35, 0x9F, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A,
		0x2E, 0x00, 0x42, 0xB9, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0A, 0x08,
		0x00, 0x1C, 0x6D,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x60, 0x00, 0x6B, 0x02, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x22, 0x00, 0x2D, 0x86, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x01, 0x31, 0x00, 0x3C, 0xA4, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x01, 0x04, 0x00, 0x0F, 0x4A, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x01, 0x40, 0x00, 0x4B, 0xC2, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x01, 0x09, 0x00, 0x14, 0x54, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0x01, 0x34, 0x00, 0x3F, 0xAA, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01,
		0x01, 0x00, 0x0C, 0x44, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02,
		0x00, 0x0D, 0x46, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x07, 0x00,
		0x12, 0x50, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x35, 0x00, 0x40,
		0xAC, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x32, 0x00, 0x3D, 0xA6,
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x06, 0x00, 0x11, 0x4E, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x00, 0x0E, 0x48, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x01, 0x30, 0x00, 0x3B, 0xA2, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x01, 0x24, 0x00, 0x2F, 0x8A, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x01, 0x23, 0x00, 0x2E, 0x88, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x01, 0x20, 0x00, 0x2B, 0x82, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0x01, 0x21, 0x00, 0x2C, 0x84, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01,
		0x11, 0x00, 0x1C, 0x64, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12,
		0x00, 0x1D, 0x66,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x30, 0x00, 0x3C, 0xA5, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x02, 0x31, 0x00, 0x3D, 0xA7, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x02, 0x10, 0x00, 0x1C, 0x65, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x02, 0x15, 0x00, 0x21, 0x6F, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x02, 0x11, 0x00, 0x1D, 0x67, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x02, 0x13, 0x00, 0x1F, 0x6B, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0x02, 0x20, 0x00, 0x2C, 0x85,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x11, 0x00, 0x28, 0x88, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0x0D, 0x16, 0x00, 0x2D, 0x92, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0x0D, 0x13, 0x00, 0x2A, 0x8C, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0x0D, 0x04, 0x00, 0x1B, 0x6E, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0x0D, 0x03, 0x00, 0x1A, 0x6C, 0xB5, 0x62, 0x06, 0x01, 0x03,
		0x00, 0x0D, 0x12, 0x00, 0x29, 0x8A, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,
		0x0D, 0x01, 0x00, 0x18, 0x68, 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x0D,
		0x06, 0x00, 0x1D, 0x72 };

uint8 UBLOX_INIT[] = { 0xB5, 0x62, 0x06, 0x01,
		0x03,
		0x00,
		0xF0,
		0x05,
		0x00,
		0xFF,
		0x19, //disable all default NMEA messages
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15, 0xB5,
		0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11, 0xB5, 0x62,
		0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F, 0xB5, 0x62, 0x06,
		0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13, 0xB5, 0x62, 0x06, 0x01,
		0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17,

		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x02, 0x01,
		0x0E,
		0x47, //set POSLLH MSG rate
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x03, 0x01,
		0x0F,
		0x49, //set STATUS MSG rate
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x06, 0x01,
		0x12,
		0x4F, //set SOL MSG rate
		0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x12, 0x01,
		0x1E,
		0x67, //set VELNED MSG rate
		0xB5, 0x62, 0x06, 0x16, 0x08, 0x00, 0x03, 0x07, 0x03, 0x00, 0x51, 0x08,
		0x00, 0x00, 0x8A,
		0x41, //set WAAS to EGNOS
		0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00,
		0xDE, 0x6A //set rate to 5Hz
		};

#define UBX_PREAMBLE1	    	0xB5	// u
#define UBX_PREAMBLE2	    	0x62	// b
#define UBX_NAV_CLASS	    0x01
#define UBX_RXM_CLASS	    0x02
#define UBX_INF_CLASS		0x04
#define UBX_ACK_CLASS		0x05
#define UBX_CFG_CLASS		0x06
#define UBX_MON_CLASS		0x0a
#define UBX_AID_CLASS	    0x0b
#define UBX_TIM_CLASS	    0x0d
#define UBX_ESF_CLASS		0x10

#define UBX_AID_REQ	    	0x00
#define UBX_TIM_TP	    	0x01

#define UBX_NAV_POSLLH	    0x02
#define UBX_NAV_STATUS	    0x03
#define UBX_NAV_DOP	    	0x04
#define UBX_NAV_SOL	    	0x06
#define UBX_NAV_VELNED	    0x12
#define UBX_NAV_TIMEUTC		0x21
#define UBX_NAV_SVINFO	    0x30
#define UBX_NAV_SBAS		0x32

#define UBX_CFG_PRT       0x00
#define UBX_CFG_MSG			0x01
#define UBX_CFG_DAT			0x09
#define UBX_CFG_TP			0x07
#define UBX_CFG_RATE		0X08
#define UBX_CFG_CFG			0X09
#define UBX_CFG_EKF			0x12
#define UBX_CFG_SBAS		0x16
#define UBX_CFG_NAV5		0x24

#define UBX_MON_VER	    	0x04
#define UBX_MON_HW	    	0x09

#define UBX_RXM_RAW	    	0x10

#define UBX_SFRB_RAW	    0x11

#define UBX_MAX_PAYLOAD   384

#define GPS_LATENCY	    75000	// us (comment out to use Ubx timepulse)
typedef struct {
	char swVersion[30];
	char hwVersion[10];
	char extension[30][7];
}__attribute__((packed)) UbxStructVER;

typedef struct {
	uint32 iTOW; // GPS Millisecond Time of Week (ms)
	int32 lon; // Longitude (deg * 1e-7)
	int32 lat; // Latitude (deg * 1e-7)
	int32 height; // Height above Ellipsoid (mm)
	int32 hMSL; // Height above mean sea level (mm)
	uint32 hAcc; // Horizontal Accuracy Estimate (mm)
	uint32 vAcc; // Vertical Accuracy Estimate (mm)
}__attribute__((packed)) UbxStructPOSLLH;

typedef struct {
	uint32 iTOW; // ms GPS Millisecond Time of Week
	uint16 gDOP; // Geometric DOP
	uint16 pDOP; // Position DOP
	uint16 tDOP; // Time DOP
	uint16 vDOP; // Vertical DOP
	uint16 hDOP; // Horizontal DOP
	uint16 nDOP; // Northing DOP
	uint16 eDOP; // Easting DOP
}__attribute__((packed)) UbxStructDOP;

typedef struct {
	uint32 time;
	int32 time_nsec;
	int16 week;
	uint8 fixtype;
	uint8 fix_status;
	int32 ecef_x; // cm
	int32 ecef_y; // cm
	int32 ecef_z; // cm
	uint32 position_accuracy_3d; // cm
	int32 ecef_x_velocity; // cm/S
	int32 ecef_y_velocity; // cm/S
	int32 ecef_z_velocity; // cm/S
	uint32 speed_accuracy; // cm/S
	uint16 position_DOP;
	uint8 res;
	uint8 satellites;
	uint32 res2;
}__attribute__((packed)) UbxStructSOL;

typedef struct {
	uint8 svid;
	uint8 flags;
	uint8 udre;
	uint8 svSys;
	uint8 svService;
	uint8 res0;
	int16 prc; // cm
	int16 res1;
	int16 ic; // cm
}__attribute__((packed)) SVStruct;

typedef struct {
	uint32 iTOW;
	uint8 geo;
	uint8 mode;
	int8 sys;
	uint8 service;
	uint8 cnt;
	uint8 res[3];
	SVStruct SV[16];
}__attribute__((packed)) UbxStructSBAS;

typedef struct {
	uint32 iTOW;
	uint8 fixtype;
	uint8 fix_status;
	uint8 flags2;
	uint32 ttff;
	uint32 msss;
}__attribute__((packed)) UbxStructSTATUS;

typedef struct {
	uint32 iTOW; // GPS Millisecond Time of Week (ms)
	int32 velN; // NED north velocity (cm/s)
	int32 velE; // NED east velocity (cm/s)
	int32 velD; // NED down velocity (cm/s)
	uint32 speed; // Speed (3-D) (cm/s)
	uint32 gSpeed; // Ground Speed (2-D) (cm/s)
	int32 heading; // Heading 2-D (deg * 1e-5)
	uint32 sAcc; // Speed Accuracy Estimate (cm/s)
	uint32 cAcc; // Course / Heading Accuracy Estimate (deg * 1e-5)
}__attribute__((packed)) UbxStructVALNED;

typedef struct {
	uint32 towMS;
	uint32 towSubMS;
	int32 qErr;
	uint16 week;
	uint8 flags;
	uint8 res;
}__attribute__((packed)) UbxStructTP;

typedef struct {
	uint32 iTOW; // GPS Millisecond Time of Week (ms)
	uint32 tAcc; // Time Accuracy Estimate
	int32 nano; // Nanosecond of second (UTC)
	uint16 year; // Year, range 1999..2099 (UTC)
	uint8 month; // Month, range 1..12 (UTC)
	uint8 day; // Day of Month, range 1..31 (UTC)
	uint8 hour; // Hour of Day, range 0..23 (UTC)
	uint8 min; // Minute of Hour, range 0..59 (UTC)
	uint8 sec; // Second of Minute, range 0..59 (UTC)
	uint8 valid; // Validity Flags
}__attribute__((packed)) UbxStructTIMEUTC;

struct {
	uint8 state;
	uint8 count;
	uint8 class;
	uint8 id;
	uint8 length;
	union {
		UbxStructVER ver;
		UbxStructPOSLLH posllh;
		UbxStructVALNED valned;
		UbxStructSOL sol;
		UbxStructSTATUS status;
		UbxStructDOP dop;
		UbxStructTP tp;
		UbxStructTIMEUTC timeutc;
		char other[UBX_MAX_PAYLOAD];
	} payload;

	uint8 RxCK_A;
	uint8 RxCK_B;

	uint8 TxUbxCK_A;
	uint8 TxUbxCK_B;
}__attribute__((packed)) ubx;

void rtcSetDataTime(int32 year, int32 month, int32 day, int32 hour,
		int32 minute, int32 second) {
	// No RTC for UAVXArm
} // rtcSetDataTime

void TxUbxu8(uint8 s, uint8 c) {
	TxChar(s, c);
	ubx.TxUbxCK_A += c;
	ubx.TxUbxCK_B += ubx.TxUbxCK_A;
} // TxUbxu8

void UbxWriteI1(uint8 s, int8 c) {
	TxChar(s, (uint8) c);
	ubx.TxUbxCK_A += c;
	ubx.TxUbxCK_B += ubx.TxUbxCK_A;
} // UbxWriteI1

void TxUbxu16(uint8 s, uint16 x) {
	TxUbxu8(s, x);
	TxUbxu8(s, x >> 8);
} // TxUbxu16

void UbxWriteI2(uint8 s, int16 x) {
	TxUbxu8(s, x);
	TxUbxu8(s, x >> 8);
} // UbxWriteI2

void TxUbxu32(uint8 s, uint32 x) {
	TxUbxu8(s, x);
	TxUbxu8(s, x >> 8);
	TxUbxu8(s, x >> 16);
	TxUbxu8(s, x >> 24);
} // TxUbxu32

void UbxWriteI4(uint8 s, int32 x) {
	TxUbxu8(s, x);
	TxUbxu8(s, x >> 8);
	TxUbxu8(s, x >> 16);
	TxUbxu8(s, x >> 24);
} // UbxWriteI4

void UbxSendPreamble(uint8 s) {

	TxUbxu8(s, UBX_PREAMBLE1); // u
	TxUbxu8(s, UBX_PREAMBLE2); // b

	ubx.TxUbxCK_A = 0;
	ubx.TxUbxCK_B = 0;
} // UbxSendPreamble

void TxUbxCheckSum(uint8 s) {

	TxChar(s, ubx.TxUbxCK_A);
	TxChar(s, ubx.TxUbxCK_B);

	TxNextLine(s); // for debug
} // TxUbxCheckSum

void UbxEnableMessage(uint8 s, uint8 Class, uint8 ID, uint8 Rate) {

	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_MSG);
	TxUbxu16(s, 3);
	TxUbxu8(s, Class);
	TxUbxu8(s, ID);
	TxUbxu8(s, Rate);
	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxEnableMessage

void UbxSetInterval(uint8 s, uint16 Interval) {

	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_RATE);
	TxUbxu16(s, 6);
	TxUbxu16(s, Interval);
	TxUbxu16(s, 0x01); // cycles
	TxUbxu16(s, 0x01); // Time Ref.	0 == UTC, 1 == GPS time
	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxSetInterval

void UbxSaveConfig(uint8 s) {

	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_CFG);
	TxUbxu16(s, 13);
	TxUbxu32(s, 0); // clear mask
	TxUbxu32(s, 0b010000011111); // save mask
	TxUbxu32(s, 0); // load mask
	TxUbxu8(s, 4); // EEPROM
	TxUbxCheckSum(s);

	Delay1mS(2000);
} // UbxSaveConfig

void UbxSetSBAS(uint8 s, uint8 enable) {

	enum Usage {
		range = 1, diffcor = 2, integrity = 4
	};
	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_SBAS);
	TxUbxu16(s, 8);
	TxUbxu8(s, enable); // disable in USA?
	TxUbxu8(s, diffcor | integrity);
	TxUbxu8(s, 3); // maxSBAS 3 was 0
	TxUbxu8(s, 0); // scan mode 2
	TxUbxu32(s, 0); // scan mode 1
	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxSetSBAS

void UbxSetMode(uint8 s) {

	enum Masks {
		dyn = 1,
		minEl = 2,
		fixMode = 4,
		drLim = 8,
		posMask = 16,
		timeMask = 32,
		staticHold = 64,
		dgpsMask = 128
	};

	enum DynamicModel {
		Portable = 0,
		Stationary = 2,
		Pedestrian = 3,
		Automotive = 4,
		Sea = 5,
		Airborne1G = 6,
		Airborne2g = 7,
		Airborne4G = 8
	};

	enum FixTypes {
		Fix2D = 1, ThreeD = 2, Automatic = 3
	};

	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_NAV5);
	TxUbxu16(s, 36);
	TxUbxu8(s, fixMode | dyn); // mask LSB (fixMode, dyn)
	TxUbxu8(s, 0b00000000); // mask MSB (reserved)
	TxUbxu8(s, Airborne1G); // Pedestrian);
	TxUbxu8(s, ThreeD); // Fix2D?
	TxUbxu32(s, 0); // altitude
	TxUbxu32(s, 10000); // 0.0001 altitude variance for 2D

	TxUbxu8(s, 5); // min elev deg (i8)
	TxUbxu8(s, 0); // dead reckoning limit s
	TxUbxu16(s, 250); // pdop x 0.1
	TxUbxu16(s, 250); // tdop x 0.1
	TxUbxu16(s, 100); // pacc m
	TxUbxu16(s, 300); // tacc m
	TxUbxu8(s, 0); // hold threshold cm/S
	TxUbxu8(s, 0); // dgps timeout
	TxUbxu32(s, 0); // reserved
	TxUbxu32(s, 0); // reserved
	TxUbxu32(s, 0); // reserved

	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxSetMode

void UbxInitPort(uint8 s) {
	UbxSendPreamble(s);

	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_PRT);

	TxUbxu8(s, 0x14); // length lsb
	TxUbxu8(s, 0x00); // length msb

	TxUbxu8(s, 0x01); // portId
	TxUbxu8(s, 0x00); // reserved
	TxUbxu16(s, 0x00); // txRead
	TxUbxu32(s, 0x000008D0); // mode
	TxUbxu32(s, UBXGPSBaud); // baudRate
	TxUbxu16(s, 0x0007); // inProtoMask
	TxUbxu16(s, 0x0001); // outProtoMask
	TxUbxu16(s, 0x00); // flags
	TxUbxu16(s, 0x00); // reserved

	Delay1mS(200);

	TxUbxCheckSum(s);
}

void UbxSetTimepulse(uint8 s) {

	UbxSendPreamble(s);
	TxUbxu8(s, UBX_CFG_CLASS);
	TxUbxu8(s, UBX_CFG_TP);
	TxUbxu16(s, 20);
	TxUbxu32(s, 1000000); // interval (us)
	TxUbxu32(s, 100000); // length (us)
#if defined(GPS_LATENCY)
	UbxWriteI1(s, 0x00); // config setting (0 == off)
#else
	UbxWriteI1(s, 0x01); // config setting (1 == +polarity)
#endif
	TxUbxu8(s, 0x01); // alignment reference time (GPS)
	TxUbxu8(s, 0x00); // bit mask (sync mode 0)
	TxUbxu8(s, 0x00); // reserved
	UbxWriteI2(s, 0x00); // antenna delay
	UbxWriteI2(s, 0x00); // rf group delay
	UbxWriteI4(s, 0x00); // user delay
	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxSetTimepulse

void UbxPollVersion(uint8 s) {
	UbxSendPreamble(s);

	TxUbxu8(s, UBX_MON_CLASS);
	TxUbxu8(s, UBX_MON_VER);

	TxUbxu8(s, 0x00); // length lsb
	TxUbxu8(s, 0x00); // length msb

	TxUbxCheckSum(s);

	Delay1mS(50);
} // UbxPollVersion

void UbxVersionSpecific(uint8 s, int16 ver) {
	if (ver > 7) // 5Hz for ver 8 using multiple GNSS
		UbxSetInterval(s, GPS_UPDATE_MS);
	else if (ver > 6) { // 10Hz for ver 7
		UbxSetInterval(s, 100);
		// SBAS screwed up on v7 modules w/ v1 firmware
		UbxSetSBAS(s, false); // disable SBAS
	} else
		// 5Hz
		UbxSetInterval(s, 200);
} // UbxVersionSpecific

void UbxBinSetup(uint8 s) {

	Delay1mS(50);

	UbxInitPort(s);
	Delay1mS(50);

	UbxSetTimepulse(s);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_VELNED, 1);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_POSLLH, 1);
	//UbxEnableMessage(s, UBX_TIM_CLASS, UBX_TIM_TP, 0);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_DOP, 5);
	UbxEnableMessage(s, UBX_AID_CLASS, UBX_AID_REQ, 1); // true
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_TIMEUTC, 255);

	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_SOL, 1); // for no of sats only

#if defined(DEBUG_GPS)
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_SVINFO, 1);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_STATUS, 1);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_SOL, 1);
	UbxEnableMessage(s, UBX_NAV_CLASS, UBX_NAV_SBAS, 1);
#endif
#if defined(GPS_DO_RTK)
	UbxEnableMessage(s, UBX_RXM_CLASS, UBX_RXM_RAW, 1);
	UbxEnableMessage(s, UBX_RXM_CLASS, UBX_SFRB_RAW, 1);
#endif

	UbxSetMode(s);
	//UbxSetInterval(s, GPS_UPDATE_MS);
	//UbxSetSBAS(s, 1);
	UbxPollVersion(s);
	UbxVersionSpecific(s, UbxVersion);

} // UbxBinSetup

void ParseUbxPacket(void) {

	enum UbxFixTypes {
		FixNone = 0,
		FixDeadReckoning = 1,
		Fix2D = 2,
		Fix3D = 3,
		FixGPSDeadReckoning = 4,
		FixTime = 5
	};

	switch (ubx.class) {
	case UBX_NAV_CLASS:
		switch (ubx.id) {
		case UBX_NAV_STATUS:
			// time
			GPS.fix = ubx.payload.status.fixtype;
			//fix_status;
			// flags2
			// tttf
			// msss
			F.GPSValid = (ubx.payload.status.fix_status & 1)
					&& ((ubx.payload.status.fixtype == Fix3D
							|| ubx.payload.status.fixtype == Fix2D));
			break;
		case UBX_NAV_SOL:
			// time
			// time_nsec
			// week
			// fixtype
			// fix_status
			// ecef_x
			// ecef_y
			// ecef_z
			// position_accuracy_3d
			// ecef_x_velocity
			// ecef_y_velocity
			// ecef_z_velocity
			// speed_accuracy
			// position_DOP
			// res
			GPS.noofsats = ubx.payload.sol.satellites;
			// res2
			break;
		case UBX_NAV_POSLLH:
			GPS.missionTime = GPS.lastPosUpdatemS = ubx.payload.posllh.iTOW;
			GPS.lat = GPS.Raw[NorthC] = (real64) ubx.payload.posllh.lat;
			GPS.lon = GPS.Raw[EastC] = (real64) ubx.payload.posllh.lon;
			GPS.height = GPS.altitude = ubx.payload.posllh.hMSL * 0.001f; // mm => m
			GPS.hAcc = ubx.payload.posllh.hAcc * 0.001f; // mm => m
			GPS.vAcc = ubx.payload.posllh.vAcc * 0.001f; // mm => m
			F.ValidGPSPos = true;
			break;
		case UBX_NAV_VELNED:
			GPS.lastVelUpdatemS = ubx.payload.valned.iTOW;
			GPS.velN = GPS.Vel[NorthC] = ubx.payload.valned.velN * 0.01f; // cm => m
			GPS.velE = GPS.Vel[EastC] = ubx.payload.valned.velE * 0.01f; // cm => m
			GPS.velD = ubx.payload.valned.velD * 0.01f; // cm => m
			GPS.gspeed = ubx.payload.valned.gSpeed * 0.01f; // cm/s => m/s
			GPS.heading = DegreesToRadians(ubx.payload.valned.heading * 1e-5f);
			GPS.sAcc = ubx.payload.valned.sAcc * 0.01f; // cm/s => m/s
			GPS.cAcc = ubx.payload.valned.cAcc * 1e-5f;
			F.ValidGPSVel = true;
			break;
		case UBX_NAV_DOP:
			GPS.pDOP = ubx.payload.dop.pDOP * 0.01f;
			GPS.hDOP = ubx.payload.dop.hDOP * 0.01f;
			GPS.vDOP = ubx.payload.dop.vDOP * 0.01f;
			GPS.tDOP = ubx.payload.dop.tDOP * 0.01f;
			GPS.nDOP = ubx.payload.dop.nDOP * 0.01f;
			GPS.eDOP = ubx.payload.dop.eDOP * 0.01f;
			GPS.gDOP = ubx.payload.dop.gDOP * 0.01f;
			break;
		case UBX_NAV_TIMEUTC:
			if (ubx.payload.timeutc.valid & 0b100) {
				rtcSetDataTime(ubx.payload.timeutc.year,
						ubx.payload.timeutc.month, ubx.payload.timeutc.day,
						ubx.payload.timeutc.hour, ubx.payload.timeutc.min,
						ubx.payload.timeutc.sec);

				GPS.year = ubx.payload.timeutc.year;
				GPS.month = ubx.payload.timeutc.month;
				GPS.day = ubx.payload.timeutc.day;

				UbxEnableMessage(GPSTxSerial, UBX_NAV_CLASS, UBX_NAV_TIMEUTC, 0); // disable message
			}
			break;
		case UBX_NAV_SBAS:

			// zzz use field names direct??
			UbxEnableMessage(GPSTxSerial, UBX_NAV_CLASS, UBX_NAV_SBAS, 0); // disable message
			break;
		case UBX_MON_CLASS:
			if (ubx.id == UBX_MON_VER) {
				UbxVersion = atoi(ubx.payload.ver.hwVersion) / 10000;
				UbxVersionSpecific(0, UbxVersion);
			}
			break;
		default:
			break;
		}
		break;
	case UBX_TIM_CLASS:
		switch (ubx.id) {
		case UBX_TIM_TP:
			GPS.TPtowMS = ubx.payload.tp.towMS;
			break;
		default:
			break;
		}
	}
	GPS.lastMessage = uSClock() * 0.000001f;

} // ParseUbxPacket

void RxUbxCheckSum(uint8 c) {
	ubx.RxCK_A += c;
	ubx.RxCK_B += ubx.RxCK_A;
} // RxUbxCheckSum

void RxUbxPacket(void) {
	uint8 c;

	while (serialAvailable(GPSRxSerial) && !F.GPSPacketReceived) {
		c = RxChar(GPSRxSerial);
		switch (RxState) {
		case WaitSentinel:
			if (c == UBX_PREAMBLE1)
				RxState = WaitSentinel2;
			break;
		case WaitSentinel2:
			if (c == UBX_PREAMBLE2)
				RxState = WaitClass;
			else
				RxState = WaitSentinel;
			break;
		case WaitClass:
			ubx.class = c;
			ubx.RxCK_A = ubx.RxCK_B = 0;
			RxUbxCheckSum(c);
			RxState = WaitID;
			break;
		case WaitID:
			ubx.id = c;
			RxUbxCheckSum(c);
			RxState = WaitLength;
			break;
		case WaitLength:
			ubx.length = c;
			RxUbxCheckSum(c);
			RxState = WaitLength2;
			break;
		case WaitLength2:
			ubx.length += (c << 8);
			RxUbxCheckSum(c);
			if (ubx.length > 0) {
				ubx.count = 0;
				RxState = WaitBody;
			} else
				RxState = WaitCheckSum;
			break;
		case WaitBody:
			*((uint8 *) (&ubx.payload) + ubx.count) = c;
			if (++ubx.count == ubx.length)
				RxState = WaitCheckSum;
			RxUbxCheckSum(c);
			break;
		case WaitCheckSum:
			if (c == ubx.RxCK_A)
				RxState = WaitCheckSum2;
			else
				RxState = WaitSentinel;
			break;
		case WaitCheckSum2:
			RxState = WaitSentinel;
			F.GPSPacketReceived = c == ubx.RxCK_B;
			if (F.GPSPacketReceived)
				ParseUbxPacket();
			break;
		default:
			RxState = WaitSentinel;
			break;
		}
	}
} // RxUbxPacket

void InitUbxGPS(uint8 s) {
	uint16 i, cs;

	for (i = 0; i < DEFAULT_BAUD_RATES; i++) {
		serialBaudRate(s, DefaultBaud[i]);
		TxChar(s, '$');
		TxCheckSum[s] = 0;
		TxGPSString(s, "PUBX,41,1,0003,000");
		TxChar(s, '1'); // UBX only
		TxChar(s, ',');
		TxVal32(s, UBXGPSBaud, 0, ASCII_NUL);
		TxString(s, ",0");
		cs = TxCheckSum[s];
		TxChar(s, '*');
		TxValH(s, cs);
		TxNextLine(s);
		Delay1mS(200);
	}

	Delay1mS(500);
	serialBaudRate(s, UBXGPSBaud);
	Delay1mS(1000);

	GPS.lag = 0.5f;

	UbxBinSetup(s);
	UbxSaveConfig(s);

} // InitUbxGPS

//___________________________________________________________________________________

// Based on MultiWii Decoders for MTK

#define PREAMBLE1_MTK16 0xd0
#define PREAMBLE1_MTK19 0xd1
#define PREAMBLE2_MTK 0xdd

#define MTK_SET_BINARY          "$PGCMD,16,0,0,0,0,0*6A\r\n"
#define MTK_SET_NMEA            "$PGCMD,16,1,1,1,1,1*6B\r\n"

#define MTK_SET_NMEA_SENTENCES  "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
//#define MTK_SET_NMEA_SENTENCES  "$PMTK314,1,1,1,1,1,5,1,1,1,1,1,1,0,1,1,1,1*2C\r\n"
//#define MTK_SET_NMEA_SENTENCES  "$PMTK314,-1*04\r\n" // defaults

#define MTK_OUTPUT_4HZ          "$PMTK220,250*29\r\n"
#define MTK_OUTPUT_5HZ          "$PMTK220,200*2C\r\n"
#define MTK_OUTPUT_10HZ         "$PMTK220,100*2F\r\n"
#define MTK_NAVTHRES_OFF        "$PMTK397,0*23\r\n" // definitely off ! velocity threshold
#define SBAS_ON                 "$PMTK313,1*2E\r\n"
#define WAAS_ON                 "$PMTK301,2*2E\r\n"
#define SBAS_TEST_MODE          "$PMTK319,0*25\r\n"  // usually PRN124 is in test
typedef struct {
	int32 latitude;
	int32 longitude;
	int32 altitude;
	int32 ground_speed;
	int32 ground_course;
	uint8 satellites;
	uint8 fixtype;
	uint32 utc_date;
	uint32 utc_time;
	uint16 hdop;
}__attribute__((packed)) MTKPacketStruct;

enum MTKFixTypes {
	FIX_NONE_MTK = 1,
	FIX_2D_MTK = 2,
	FIX_3D_MTK = 3,
	FIX_2D_SBAS_MTK = 6,
	FIX_3D_SBAS_MTK = 7
};

union {
	MTKPacketStruct msg;
	uint8 bytes[32];
} MTKBuffer;

boolean UseMTK16GPS = false;

void ParseMTKPacket(void) {

	F.GPSValid = ((MTKBuffer.msg.fixtype == FIX_3D_MTK)
			|| (MTKBuffer.msg.fixtype == FIX_3D_SBAS_MTK));

	if (UseMTK16GPS) {
		GPS.Raw[NorthC] = MTKBuffer.msg.latitude * 10;
		GPS.Raw[EastC] = MTKBuffer.msg.longitude * 10;
	} else {
		GPS.Raw[NorthC] = (real64) MTKBuffer.msg.latitude;
		GPS.Raw[EastC] = (real64) MTKBuffer.msg.longitude;
	}
	GPS.altitude = MTKBuffer.msg.altitude * 0.01f;
	GPS.gspeed = MTKBuffer.msg.ground_speed * 0.01f; // in m/s * 100 == in cm/s

	GPS.heading = DegreesToRadians(MTKBuffer.msg.ground_course);
	GPS.noofsats = MTKBuffer.msg.satellites;
	GPS.fix = MTKBuffer.msg.fixtype;
	GPS.lastVelUpdatemS = GPS.lastPosUpdatemS = MTKBuffer.msg.utc_time;

	GPS.hDOP = MTKBuffer.msg.hdop * 0.01f;
	GPS.hAcc = GPS.vAcc = GPS.hDOP * GPS_HDOP_TO_HACC; // TODO: kludge

	F.ValidGPSVel = F.ValidGPSPos = true;

} // ParseMTKPacket

void RxMTKPacket(void) {
	static uint8 _ck_b, _ck_a;
	static int16 cc = 0;
	uint8 c;

	while (serialAvailable(GPSRxSerial) && !F.GPSPacketReceived) {
		c = RxChar(GPSRxSerial);
		switch (RxState) {
		case WaitSentinel:
			if (c == PREAMBLE1_MTK16) {
				UseMTK16GPS = true;
				RxState = WaitID;
			} else if (c == PREAMBLE1_MTK19) {
				UseMTK16GPS = false;
				RxState = WaitID;
			} else
				RxState = WaitSentinel;
			break;
		case WaitSentinel2:
			if (c == PREAMBLE2_MTK)
				RxState = WaitLength;
			else
				RxState = WaitSentinel;
			break;
		case WaitLength:
			if (sizeof(MTKBuffer.msg) == c) {
				RxState = WaitBody;
				_ck_b = _ck_a = c;
				cc = 0;
			} else
				RxState = WaitSentinel;
			break;
		case WaitBody:
			MTKBuffer.bytes[cc++] = c;
			_ck_b += (_ck_a += c);
			if (cc == sizeof(MTKBuffer.msg))
				RxState = WaitCheckSum;
			break;
		case WaitCheckSum:
			if (c == _ck_a)
				RxState = WaitCheckSum2;
			else
				RxState = WaitSentinel;
			break;
		case WaitCheckSum2:
			F.GPSPacketReceived = c == _ck_b;
			if (F.GPSPacketReceived)
				ParseMTKPacket();
			RxState = WaitSentinel;
			break;
		} // switch
	}
} // RxMTKPacket

void InitMTKGPS(uint8 s, boolean UseNMEA) {
	uint8 i, cs;

	for (i = 0; i < DEFAULT_BAUD_RATES; i++) {
		serialBaudRate(s, DefaultBaud[i]);
		TxChar(s, '$');
		TxCheckSum[s] = 0;
		TxGPSString(s, "PMTK251,");
		TxVal32(s, GPSBaud, 0, ASCII_NUL);
		cs = TxCheckSum[s];
		TxChar(s, '*');
		TxValH(s, cs);
		TxNextLine(s);
		Delay1mS(20);
	}

	serialBaudRate(s, GPSBaud);

	TxGPSString(s, MTK_NAVTHRES_OFF);
	Delay1mS(80);
	TxGPSString(s, SBAS_ON);
	Delay1mS(80);
	TxGPSString(s, WAAS_ON);
	Delay1mS(80);
	TxGPSString(s, SBAS_TEST_MODE); // TODO: is this still current
	Delay1mS(80);
	TxGPSString(s, MTK_OUTPUT_5HZ);

	if (UseNMEA)
		TxGPSString(s, MTK_SET_NMEA_SENTENCES); // only GGA and RMC sentence
	else
		TxGPSString(s, MTK_SET_BINARY);

	GPS.lag = 0.5f;

} // InitMTKGPS

//___________________________________________________________________________________

// NMEA Decoder

real32 GPSToM(int32 c) {
	//return ((real64) c * 0.011131948079f);
	return ((real64) c * ((real64) EARTH_RADIUS_M * PI) / (180.0 * 1e7));
} // GPSToM

int32 MToGPS(real32 c) {
	return ((real64) c / (((real64) EARTH_RADIUS_M * PI) / (180.0 * 1e7)));
} // MToGPS

int32 I32(uint8 lo, uint8 hi) {
	uint8 i;
	int32 r;

	r = 0;
	if (!EmptyField)
		for (i = lo; i <= hi; i++)
			r = r * 10 + NMEA.s[i] - '0';

	return (r);
} // I32


int32 ConvertLatLon(uint8 lo, uint8 hi) {
	int32 dd, mm, dm, r;
	uint8 dp;

	r = 0;
	if (!EmptyField) {
		dp = lo + 4;
		while (NMEA.s[dp] != '.')
			dp++;

		dd = I32(lo, dp - 3);
		mm = I32(dp - 2, dp - 1);
		if ((hi - dp) > (uint8) 4)
			dm = I32(dp + 1, dp + 5);
		else
			dm = I32(dp + 1, dp + 4) * 10L;

		r = dd * 10000000;
		r += (mm * 10000000 + dm * 100 + 30) / 60;
	}

	return (r);
} // ConvertLatLon


int32 ConvertUTime(uint8 lo, uint8 hi) {
	int32 ival;

	ival = 0;
	if (!EmptyField)
		ival = (int32) (I32(lo, lo + 1)) * 3600 + (int32) (I32(lo + 2, lo + 3)
				* 60) + (int32) (I32(lo + 4, hi));

	return (ival);
} // ConvertUTime

void UpdateField(void) {
	uint8 ch;

	lo = cc;

	ch = NMEA.s[cc];
	while ((ch != ',') && (ch != '*') && (cc < nll))
		ch = NMEA.s[++cc];

	hi = cc - 1;
	cc++;
	EmptyField = hi < lo;
} // UpdateField

void ParseGXGGASentence(void) { // full position $GXGGA fix
	uint32 PacketTimemS;

	cc = 0;
	nll = NMEA.length;

	UpdateField();

	UpdateField(); //UTime
	PacketTimemS = mSClock(); // ConvertUTime(lo, hi);

	UpdateField(); //Lat
	GPS.Raw[NorthC] = ConvertLatLon(lo, hi);
	UpdateField(); //LatH
	if (NMEA.s[lo] == 'S')
		GPS.Raw[NorthC] = -GPS.Raw[NorthC];

	UpdateField(); //Lon
	GPS.Raw[EastC] = ConvertLatLon(lo, hi);
	UpdateField(); //LonH
	if (NMEA.s[lo] == 'W')
		GPS.Raw[EastC] = -GPS.Raw[EastC];

	UpdateField(); //Fix
	GPS.fix = (uint8) (I32(lo, hi));

	UpdateField(); //Sats
	GPS.noofsats = (uint8) (I32(lo, hi));

	UpdateField(); // HDOP
	GPS.hDOP = (real32) I32(lo, hi - 3) + (real32) (I32(hi - 1, hi)) * 0.01;
	GPS.hAcc = GPS.vAcc = GPS.hDOP * GPS_HDOP_TO_HACC;

	UpdateField(); // Alt
	GPS.altitude = (real32) (I32(lo, hi - 2)) + (real32) (I32(hi, hi)) * 0.1f;

	//UpdateField();   // AltUnit - assume Metres!

	//UpdateField();   // GHeight
	//GPS.geoidheight = (real32) (I32(lo, hi - 2)) + (real32) (I32(hi, hi)) * 0.1f;
	//UpdateField();   // GHeightUnit

	F.GPSValid = (GPS.fix > 0) && (GPS.noofsats >= GPS_MIN_SATELLITES);
	if (F.GPSValid)
		GPS.missionTime = GPS.lastPosUpdatemS = PacketTimemS;

} // ParseGXGGASentence

void ParseGXRMCSentence() { // main current position and heading
	uint32 PacketTimemS;

	cc = 0;
	nll = NMEA.length;

	UpdateField();

	UpdateField(); //UTime
	PacketTimemS = mSClock(); //ConvertUTime(lo, hi);

	UpdateField();
	if (NMEA.s[lo] == 'A') {

		UpdateField(); //Lat
		GPS.Raw[NorthC] = ConvertLatLon(lo, hi);
		UpdateField(); //LatH
		if (NMEA.s[lo] == 'S')
			GPS.Raw[NorthC] = -GPS.Raw[NorthC];

		UpdateField(); //Lon
		GPS.Raw[EastC] = ConvertLatLon(lo, hi);
		UpdateField(); //LonH
		if (NMEA.s[lo] == 'W')
			GPS.Raw[EastC] = -GPS.Raw[EastC];

		UpdateField(); // Groundspeed (Knots)
		GPS.gspeed = ((real32) I32(lo, hi - 3) + (real32) I32(hi - 1, hi)
				* 0.01) * 0.5144444; //  MPS/Kt

		UpdateField(); // True course made good (Degrees)
		GPS.heading = DegreesToRadians((real32)I32(lo, hi - 3) + (real32)I32(
						hi - 1, hi) * 0.01);

		UpdateField();
		GPS.day = I32(lo, lo + 1);
		GPS.month = I32(lo + 2, lo + 3);
		GPS.year = I32(lo + 4, lo + 5) + 2000;

		/* usually returns zero - not used
		 UpdateField();
		 GPS.magvariation = DegreesToRadians((real32) I32(lo, hi) * 0.1f);
		 UpdateField(); // magvar sign
		 if (NMEA.s[lo] == 'W')
		 GPS.magvariation = -GPS.magvariation;
		 */

		GPS.missionTime = GPS.lastVelUpdatemS = PacketTimemS;
	}

} // ParseGXRMCSentence


void SetGPSOrigin(void) {
	uint8 i;

	if ((ValidGPSSentences == GPS_ORIGIN_SENTENCES) && !F.OriginValid) {

		mS[LastGPS] = mSClock();

		GPS.startTime = GPS.missionTime;

		NV.Mission.OriginLatitude = GPS.OriginRaw[NorthC] = GPS.Raw[NorthC];
		NV.Mission.OriginLongitude = GPS.OriginRaw[EastC] = GPS.Raw[EastC];

		GPS.Vel[NorthC] = GPS.Vel[EastC] = 0.0f;

		// do it once - operations area small
		GPS.longitudeCorrection
				= Abs(cos(DegreesToRadians((real64)GPS.Raw[NorthC] * 1e-7)));
		GPS.PosP[NorthC] = GPS.PosP[EastC] = 0.0f;

		GPS.originAltitude = GPS.altitude;

		NV.Mission.OriginAltitude = GPS.altitude;
		NV.Mission.OriginLatitude = GPS.OriginRaw[NorthC];
		NV.Mission.OriginLongitude = GPS.OriginRaw[EastC];

		ResetNavHold = true;
		F.NavigationActive = false;
		NavState = PIC;

		if (!F.OriginValid) {
			for (i = 0; i < 8; i++)
				DoBeep(2, 2);

			NV.Stats[OriginValidS] = true;
			F.OriginValid = true;
		}
	}
} // SetGPSOrigin

boolean GPSSanityCheck(void) {
	boolean r;

	r = F.Emulation ? true : (Abs(GPS.Raw[NorthC]) <= 900000000L)
			&& (Abs(GPS.Raw[EastC]) <= 1800000000L) && (GPS.Raw[NorthC] != 0)
			&& (GPS.Raw[EastC] != 0);

	if (!r)
		NV.Stats[GPSInvalidS]++;

	return (r);

} // GPSSanityCheck

void ProcessGPSSentence(void) {
	int32 a;

	if (F.GPSValid && GPSSanityCheck()) {

		GPSdT = (real32) (GPS.lastPosUpdatemS - LastGPSUpdatemS) * 0.001f;
		//zzz GPSdT = Limit(GPSdT, 0.2, 0.5f);
		GPSdTR = 1.0f / GPSdT;
		LastGPSUpdatemS = GPS.lastPosUpdatemS;

		if (F.OriginValid) {
			if (!F.Emulation) {
				GPS.Pos[NorthC] = GPSToM(GPS.Raw[NorthC]
						- GPS.OriginRaw[NorthC]);
				GPS.Pos[EastC] = GPSToM(GPS.Raw[EastC] - GPS.OriginRaw[EastC])
						* GPS.longitudeCorrection;

				if ((CurrGPSType != UBXBinGPS)
						&& (CurrGPSType != UBXBinGPSInit)) {
					for (a = NorthC; a <= EastC; a++) {
						GPS.Vel[a] = (GPS.Pos[a] - GPS.PosP[a]) * GPSdT;
						GPS.PosP[a] = GPS.Pos[a];
					}
					GPS.gspeed = sqrtf(Sqr(GPS.Vel[NorthC])
							+ Sqr(GPS.Vel[EastC]));
				}
			}

			StatsMax(GPS.altitude, GPSAltitudeS);
			StatsMax(GPS.gspeed * 10.0f, GPSVelS);
			StatsMinMax(GPS.hAcc * 100.0f, MinhAccS, MaxhAccS);
			StatsMinMax(GPS.noofsats, GPSMinSatsS, GPSMaxSatsS);

		} else {
			F.GPSValid = false;
			if (ValidGPSSentences < GPS_ORIGIN_SENTENCES)
				ValidGPSSentences
						= GPS.hAcc <= GPS_MIN_HACC ? ValidGPSSentences + 1 : 0;
		}
	} else {
		//GPS.Vel[NorthC] = GPS.Vel[EastC] = 0.0f;
		F.ValidGPSVel = false;
		if (State == InFlight)
			NV.Stats[GPSInvalidS]++;

	}

	F.GPSFailure = GPS.hAcc > GPS_MIN_HACC;

} // ProcessGPSSentence

void RxNMEAPacket(void) {
	uint8 c;

	while (serialAvailable(GPSRxSerial) && !F.GPSPacketReceived) {
		c = RxChar(GPSRxSerial);
		switch (RxState) {
		case WaitSentinel:
			if (c == '$') {
				ll = tt = ss = RxCheckSum = 0;
				RxState = WaitID;
			}
			break;
		case WaitID:
			RxCheckSum ^= c;
			while ((c != NMEATags[ss][tt]) && (ss < MAX_NMEA_SENTENCES))
				ss++;
			if (c == NMEATags[ss][tt])
				if (tt == NMEA_TAG_INDEX) {
					GPSPacketTag = ss;
					RxState = WaitBody;
				} else
					tt++;
			else
				RxState = WaitSentinel;
			break;
		case WaitBody:
			if (c == '*')
				RxState = WaitCheckSum;
			else if (c == '$') {
				ll = tt = RxCheckSum = 0;
				RxState = WaitID;
			} else {
				RxCheckSum ^= c;
				NMEA.s[ll++] = c;
				if (ll > (GPSRXBUFFLENGTH - 1))
					RxState = WaitSentinel;
			}
			break;
		case WaitCheckSum:
			if (c >= 'A')
				GPSTxCheckSum = c - ('A' - 10);
			else
				GPSTxCheckSum = c - '0';
			RxState = WaitCheckSum2;
			break;
		case WaitCheckSum2:
			GPSTxCheckSum *= 16;
			if (c >= 'A')
				GPSTxCheckSum += c - ('A' - 10);
			else
				GPSTxCheckSum += c - '0';
			NMEA.length = ll;
			F.GPSPacketReceived = GPSTxCheckSum == RxCheckSum;
			if (F.GPSPacketReceived)
				switch (GPSPacketTag) {
				case GPGGAPacketTag:
				case GNGGAPacketTag:
					ParseGXGGASentence();
					break;
				case GPRMCPacketTag:
				case GNRMCPacketTag:
					ParseGXRMCSentence();
					break;
				default:
					F.GPSPacketReceived = false;
				} // switch
			RxState = WaitSentinel;
			break;
		} // switch
	}
} // RxNMEAPacket

void UpdateGPS(void) {
	uint32 NowmS;

	if (F.Emulation)
		GPSEmulation(); // real GPS preferably unplugged
	else {
		switch (CurrGPSType) {
		case UBXBinGPS:
		case UBXBinGPSInit:
			RxUbxPacket();
			break;
		case MTKBinGPS:
			RxMTKPacket();
			break;
		case MTKNMEAGPS:
		case NMEAGPS:
			RxNMEAPacket();
			break;
		default:
			// Rx disabled elsewhere
			break;
		} // switch
	}

	NowmS = mSClock();
	if (F.GPSPacketReceived) {
		F.GPSPacketReceived = false;

		if (GPS.lastPosUpdatemS > LastGPSUpdatemS) {

			F.HaveGPS = true;

			LEDOff(LEDRedSel);
			LEDToggle(LEDBlueSel);

			ProcessGPSSentence();

			F.NewGPSPosition = F.GPSValid && F.OriginValid;
			if (F.NewGPSPosition)
				mSTimer(NowmS, GPSTimeout, GPS_TIMEOUT_MS);
		}

	} else {
		if (NowmS > mS[GPSTimeout]) {
			DecayPosCorr();

			LEDOff(LEDBlueSel);
			LEDOn(LEDRedSel);
		}
	}

} // UpdateGPS


void InitGPS(void) {

	cc = 0;
	memset(&GPS, 0, sizeof(GPS));

	mS[FakeGPSUpdate] = 0;
	F.OriginValid = F.GPSValid = F.HaveGPS = F.GPSPacketReceived = false;
	ValidGPSSentences = 0;

	GPS.year = 0; // no auto variation with MTK

	//	GPS.lag = 1.0f;

	//	GPS.Raw[NorthC] = GPS.C[NorthC].OriginRaw = 0;
	// done in memset	GPS.Raw[EastC] = GPS.C[EastC].OriginRaw = 0;

	if (F.Emulation) {
		GPS.OriginRaw[NorthC] = DEFAULT_HOME_LAT;
		GPS.OriginRaw[EastC] = DEFAULT_HOME_LON;
		GPS.longitudeCorrection = DEFAULT_LON_CORR;
	}

	LEDOn(LEDBlueSel);

	RxEnabled[GPSRxSerial] = false;
	switch (CurrGPSType) {
	case UBXBinGPSInit:
		InitUbxGPS(GPSTxSerial);
		break;
	case MTKBinGPS:
		InitMTKGPS(GPSTxSerial, false);
		break;
	case MTKNMEAGPS:
		InitMTKGPS(GPSTxSerial, true);
		break;
	case NMEAGPS:
	default:
		break;
	} // switch

	LEDOff(LEDBlueSel);

	serialBaudRate(GPSRxSerial, GPSBaud);

	RxEnabled[GPSRxSerial] = true;

	RxState = WaitSentinel;

} // InitGPS


