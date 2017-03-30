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


#include "UAVX.h"

volatile uint32 TicksuS;

void cycleCounterInit(void) {
	RCC_ClocksTypeDef clocks;

	RCC_GetClocksFreq(&clocks);
	TicksuS = clocks.SYSCLK_Frequency / 1000000;

	// enable DWT access
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// enable the CPU cycle counter
	DWT_CTRL |= CYCCNTENA;
} // cycleCounterInit

uint32 uSClock(void) {
	// TODO: no check for wraparound

	register uint32 PrevTick, Tick, mS;
	__disable_irq();
	Tick = *DWT_CYCCNT;
	PrevTick = sysTickCycleCounter;
	mS = sysTickUptime;
	__enable_irq();
	return ((mS * 1000) + (Tick - PrevTick) / TicksuS);

} // uSClock

void Delay1uS(uint16 d) {
	// TODO: needs round up
	uint32 TimeOut;

	TimeOut = uSClock() + d;
	while (uSClock() < TimeOut) {
	};

} // Delay1uS

uint32 mSClock(void) {
	return (sysTickUptime);
} // mSClock

void Delay1mS(uint16 d) {
	// TODO: needs round up
	uint32 TimeOut;

	TimeOut = mSClock() + d + 1; // clock may be rolling over
	while (mSClock() < TimeOut) {
	};

} // Delay1mS

real32 dTUpdate(uint32 NowuS, uint32 * LastUpdateuS) {
	real32 dT;

	NowuS = uSClock();
	dT = (NowuS - *LastUpdateuS) * 0.000001f;
	*LastUpdateuS = NowuS;

	return (dT);
} // dtUpdate


void mSTimer(uint32 NowmS, uint8 t, int32 TimePeriod) {
	mS[t] = NowmS + TimePeriod;
} // mSTimer

void uSTimer(uint32 NowuS, uint8 t, int32 TimePeriod) {
	uS[t] = NowuS + TimePeriod;
} // uSTimer


void InitClocks(void) {
	cycleCounterInit();
	SysTick_Config(SystemCoreClock / 1000);
} // InitClocks

