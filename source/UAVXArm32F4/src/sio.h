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


#ifndef _sio_h
#define _sio_h

extern boolean sioReadBlock(uint8 sioDev, uint8 id, uint8 reg, uint8 len,
		uint8 * data);
extern boolean sioWriteBlock(uint8 sioDev, uint8 id, uint8 reg, uint8 len,
		uint8 * data);

extern uint8 sioRead(uint8 sioDev, uint8 id, uint8 reg);
extern uint8 sioReadataddr(uint8 sioDev, uint8 id, uint8 reg);
extern boolean sioReadBlockataddr(uint8 sioDev, uint8 id, uint8 reg, uint8 len,
		uint8 * data);
extern boolean sioReadBlocki16v(uint8 sioDev, uint8 id, uint8 len,
		int16 * data, boolean h);
extern boolean sioReadBlocki16vataddr(uint8 sioDev, uint8 id, uint8 reg,
		uint8 len, int16 * data, boolean h);

extern boolean sioWrite(uint8 sioDev, uint8 id, uint8 reg, uint8 data);
//extern boolean sioWritebyte(uint8 sioDev, uint8 id, uint8 v);
extern boolean sioWriteataddr(uint8 sioDev, uint8 id, uint8 a, uint8 v);
extern boolean sioWriteBlockataddr(uint8 sioDev, uint8 id, uint8 reg,
		uint8 len, uint8 * data);

extern boolean sioResponse(uint8 sioDev, uint8 id);

#endif


