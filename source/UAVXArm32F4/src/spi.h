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


#ifndef _spi_h
#define _spi_h

typedef struct {
	boolean ClockHigh;
	uint32 ReadRate;
	uint32 WriteRate;
} spiDefStruct;

extern const spiDefStruct spiDef[];

extern void spiSelect(uint8 devSel, boolean Sel);
extern SPI_TypeDef * spiSetBaudRate(uint8 devSelS, boolean R);
extern void spiSetDivisor(SPI_TypeDef *SPIx, uint16 d);

extern uint8 spiSend(SPI_TypeDef *SPIx, uint8 d);

extern boolean spiReadBlock(uint8 devSel, uint8 id, uint8 reg, uint8 len,
		uint8 * data);
extern boolean spiWriteBlock(uint8 devSel, uint8 id, uint8 reg, uint8 len,
		uint8 * data);

extern uint32 spiErrors;

#endif

