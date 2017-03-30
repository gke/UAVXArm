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


#ifndef _i2c_h
#define _i2c_h

#define I2C_CLOCK_HZ 400000

typedef struct { // TODO: possibly combine with Port def
	boolean error;
	boolean busy;
	uint32 i2cErrors;
	boolean subaddress_sent;
	boolean final_stop;
	boolean writing;
	boolean reading;
	uint8 addr; // 8bit
	uint8 reg;
	uint16 bytes;
	uint8* write_p;
	uint8* read_p;
} i2cStateDef;

boolean i2cReadBlock(uint8 devSel, uint8 id, uint8 reg, uint8 l,
		uint8 *data);
boolean i2cWriteBlock(uint8 devSel, uint8 id, uint8 reg, uint8 len,
		uint8 *data);

boolean i2cResponse(uint8 devSel, uint8 d);

void i2c_er_handler(uint8 i2cCurr);
void i2c_ev_handler(uint8 i2cCurr);

void i2cInit(uint8 I2CCurr);

extern volatile i2cStateDef i2cState[];


#endif

