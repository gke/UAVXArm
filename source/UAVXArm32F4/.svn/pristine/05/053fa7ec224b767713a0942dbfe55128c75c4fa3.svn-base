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


#ifndef _blackbox_h
#define _blockbox_h


#if defined(V4_BOARD)
#define MEM_BLOCK_SIZE 256  //256 page size for external Flash
#else
#define MEM_BLOCK_SIZE 32  // 64 max i2c driver limitation
#endif

extern void BlackBox(uint8 ch);
extern void UpdateBlackBox(void);
extern void DumpBlackBox(uint8 s);
extern void InitBlackBox(void);


extern boolean BlackBoxEnabled;
extern uint8 BBReplaySpeed;

#endif


