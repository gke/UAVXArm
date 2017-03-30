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


#ifndef _spiflash_h
#define _spiflash_h

#define FLAG_READY			7
#define FLAG_STALE			6
#define FLAG_PROTECTED		1
#define FLAG_256			0

#define FLAG_EPE			5

boolean flashInit(void);
boolean flashConfig256(uint8 devSel);
void flashReset(uint8 devSel);
uint8 flashReadStatus(uint8 devSel);
boolean flashFlagSet(uint8 devSel, uint8 fb);
boolean flashDeviceInfoValid(uint8 devSel);
void flashShowStatus(uint8 s);

boolean flashErasePage(uint8 devSel, uint32 a);
boolean flashEraseBlock(uint8 devSel, uint32 a);
boolean flashErase(uint8 devSel);

boolean flashReadPage(uint8 devSel, uint32 flashAddr, uint32 len, int8 *data);
boolean flashReadModifyWrite(uint8 devSel, uint32 a, uint32 len, int8 *data);

extern uint8 flashInfo[];
extern uint16 flashLastStatus;

#endif
