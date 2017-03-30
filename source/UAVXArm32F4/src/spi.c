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

//    UAVX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "UAVX.h"

uint32 spiErrors = 0;

SPI_TypeDef * spiSetBaudRate(uint8 devSel, boolean R) {
	// It would be good if there was some consistency with SPI protocols!!!
	// All of this for the HMC5983.

	//static uint16 lastDevClockHigh = false;
	uint16 devRate;
	SPI_TypeDef * SPIx;

	SPIx = SPIPorts[spiMap[devSel] - 1].SPIx;

	SPI_Cmd(SPIx, DISABLE);

	devRate = R ? spiDef[devSel].ReadRate : spiDef[devSel].WriteRate;
	SPIx->CR1 = (SPIx->CR1 & 0b1111111111000111) | devRate;
	/*
	 if (spiDef[devSel].ClockHigh != lastDevClockHigh) {
	 lastDevClockHigh = spiDef[devSel].ClockHigh;
	 spiInitGPIOPins(spiMap[devSel], lastDevClockHigh);

	 if (spiDef[devSel].ClockHigh)
	 SPIx->CR1 = (SPIx->CR1 ^ (SPI_CPOL_Low & SPI_CPHA_1Edge))
	 | (SPI_CPOL_High & SPI_CPHA_2Edge);
	 else
	 SPIx->CR1 = (SPIx->CR1 ^ (SPI_CPOL_High & SPI_CPHA_2Edge))
	 | (SPI_CPOL_Low & SPI_CPHA_1Edge);

	 Delay1uS(5); // ???? zzzz
	 }
	 */
	SPI_Cmd(SPIx, ENABLE);

	return (SPIx);

} // spiSetBaudRate


void spiSelect(uint8 devSel, boolean Sel) {

	if (Sel) {
		Delay1uS(1);
		digitalWrite(&SPISelectPins[devSel], 0);
	} else
		digitalWrite(&SPISelectPins[devSel], 1);

} // spiSelect

uint8 spiSend(SPI_TypeDef * SPIx, uint8 d) {

	while (!(SPIx->SR & SPI_I2S_FLAG_TXE)) {
	};

	SPIx->DR = d;

	while (!(SPIx->SR & SPI_I2S_FLAG_RXNE)) {
	};

	while (SPIx->SR & SPI_I2S_FLAG_BSY) {
	};

	return SPIx->DR;
} // spiSend

uint8 spiSendzzz(SPI_TypeDef * SPIx, uint8 d) {
	uint16 spiTimeout;

	SPI_I2S_SendData(SPIx, d);

	spiTimeout = 0x1000;

	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) {
		if ((spiTimeout--) == 0) {
			spiErrors++;
			NV.Stats[SPIFailS] = spiErrors;
			return (0);
		}
		//Delay1uS(2); ???
	}

	spiTimeout = 0x1000;

	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
		if ((spiTimeout--) == 0) {
			F.spiFatal |= true;
			spiErrors++;
			NV.Stats[SPIFailS] = spiErrors;
			return (0);
		}
		//Delay1uS(2); ???
	}

	return ((uint8) SPI_I2S_ReceiveData(SPIx)); // a read for every write in full duplex

} // spiSend

boolean spiReadBlock(uint8 devSel, uint8 id, uint8 d, uint8 len, uint8* data) {
	uint8 i;
	SPI_TypeDef * s;
	uint32 r;
	uint8 Prefix;

	r = spiErrors;

	// KLUDGE

	if (devSel == mpu60xxSel)
		Prefix = 0x80;
	else if (devSel == hmc5xxxSel) {
		if (len > 1)
			Prefix = 0xc0;
		else
			Prefix = 0x80;
	} else
		Prefix = 0;

	s = spiSetBaudRate(devSel, true);

	spiSelect(devSel, true);
	spiSend(s, Prefix | d); // MANY devices do not use Read if MSB set so do not OR in here

	for (i = 0; i < len; i++)
		data[i] = spiSend(s, 0);

	spiSelect(devSel, false);

	return (r == spiErrors);

} // spiReadBlock


boolean spiWriteBlock(uint8 devSel, uint8 id, uint8 d, uint8 len, uint8 *data) {
	uint8 i;
	SPI_TypeDef * s;
	uint32 r;

	r = spiErrors;

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	spiSend(s, d);

	for (i = 0; i < len; i++)
		spiSend(s, data[i]);

	spiSelect(devSel, false);

	return (r == spiErrors);

} // spiWriteBlock

