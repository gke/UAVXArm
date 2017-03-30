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


#ifndef _invensense_h
#define _invensense_h

#define MPU_0x68_ID (0x68*2)
#define MPU_0x69_ID (0x69*2)

extern uint8 MPU6XXXId;
extern uint8 MPU6XXXRev;
extern const uint16 MPULPFHz[];
extern const uint16 MPUAccLPFHz[];
extern const uint8 MPUDLPFMask[];
extern const char * DHPFName[];

extern uint8 MPU6XXXDLPF;
extern uint8 MPU6XXXDHPF;
extern uint8 MPU6XXXAccDLPF;
extern real32 MPU6XXXTemperature;

void CalibrateAccAndGyro(uint8 s);
void InitMPU6XXX(void);
void CheckMPU6XXXActive(void);
void ReadAccAndGyro(boolean UseSelectedAttSensors);
void UpdateAccAndGyroBias(void);

extern uint8 MPU_ID;
extern uint32 mpu6xxxLastUpdateuS;
extern real32 RawAcc[], RawGyro[];

#endif


