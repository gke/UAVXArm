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


#ifndef _battery_h
#define _battery_h

#define BATTERY_UPDATE_MS 200 // 1000
#define LVC_FULL_BUCKET_MS 3000
#define LVC_INIT_WARN_SCALE FromPercent(90, RC_MAXIMUM)
#define LVC_THR_OFF_MS 10000
#define LVC_DECAY_STEP ((real32)BATTERY_UPDATE_MS/LVC_THR_OFF_MS)

extern void InitBattery(void);
extern void CheckBatteries(void);
extern void BatteryTest(uint8 s);

extern real32 BatteryVolts, BatterySagR, BatteryCurrent, BatteryVoltsLimit, StartupVolts,
		BatteryChargeUsedmAH, BatteryCapacitymAH;
extern uint8 BatteryCellCount;
extern real32 BatteryCurrentADCZero;

#endif


