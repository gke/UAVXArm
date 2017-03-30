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

// USART routines

#include "UAVX.h"

#define BUFFER_SIZE 2048
#define BUFFER_MASK (BUFFER_SIZE-1)
uint32 CurrExtMemAddr;
uint32 LastExtMemAddr = MEM_SIZE;
int8 BBQ[BUFFER_SIZE];
volatile uint16 BBQHead;
volatile uint16 BBQTail;
volatile uint16 BBQEntries;
boolean BlackBoxEnabled = false;
uint8 BBReplaySpeed = 1;

void UpdateBlackBox(void) {
	int8 B[MEM_BLOCK_SIZE];
	uint16 i;

	if (F.HaveExtMem)
		if ((uSClock() >= uS[MemReady]) && (BBQEntries >= MEM_BLOCK_SIZE)) {

			for (i = 0; i < MEM_BLOCK_SIZE; i++) {
				BBQHead = (BBQHead + 1) & BUFFER_MASK;
				B[i] = BBQ[BBQHead];
			}
			WriteBlockExtMem(CurrExtMemAddr, MEM_BLOCK_SIZE, B);
			BBQEntries -= MEM_BLOCK_SIZE;

			CurrExtMemAddr += MEM_BLOCK_SIZE;
			if (CurrExtMemAddr > LastExtMemAddr) // wrap around to capture end of flight
				CurrExtMemAddr = 0;

		}
} // UpdateBlackBox


void BlackBox(uint8 ch) {
	uint16 NewTail;

	if (BlackBoxEnabled && Armed() && (State == InFlight)) {
		NewTail = (BBQTail + 1) & BUFFER_MASK;
		BBQ[NewTail] = ch;
		BBQTail = NewTail;
		BBQEntries++;
	}

} // BlackBox

void DumpBlackBox(uint8 s) {
	int32 seqNo;
	int32 a;
	uint32 MaxMemoryUsed;
	int8 B[MEM_BLOCK_SIZE];
	uint16 i;
	boolean Finish;

#if defined(V4_BOARD)
	MaxMemoryUsed = MEM_SIZE; // Read32ExtMem(0);
	a = MEM_BLOCK_SIZE;
#else
	MaxMemoryUsed = MEM_SIZE;
	a = 0;
#endif

	F.DumpingBlackBox = true;

	seqNo = 0;

	if (F.HaveExtMem) {

		for (i = 0; i < 10; i++)
			SendFlightPacket(s);

		do {
			ReadBlockExtMem(a, MEM_BLOCK_SIZE, B);
			Finish = true;
			for (i = 0; i < MEM_BLOCK_SIZE; i++)
				Finish &= B[i] == -1;
			if (!Finish) {
				if (MEM_BLOCK_SIZE > 128) {
					SendBBPacket(s, seqNo++, 128, &B[0]);
					SendBBPacket(s, seqNo++, MEM_BLOCK_SIZE - 128, &B[128]);
				} else
					SendBBPacket(s, seqNo++, (uint8)MEM_BLOCK_SIZE, &B[0]);
				a += MEM_BLOCK_SIZE;
			}
		} while ((a < MaxMemoryUsed) && !Finish);

		B[0] = 0;
		SendBBPacket(s, -1, 1, B);
	}

	F.DumpingBlackBox = false;

} // DumpBlackBox


void InitBlackBox(void) {

	BBQHead = BBQTail = BBQEntries = 0;
	CurrExtMemAddr = 0;

} // InitBlackBox
