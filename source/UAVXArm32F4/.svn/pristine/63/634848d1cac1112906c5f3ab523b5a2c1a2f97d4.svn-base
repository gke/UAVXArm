// ===============================================================================================
// =                                UAVX Quadrocopter ContRoller                                 =
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

//    You should have received a copy of the GNU General Public License aint32 with this program.
//    If not, see http://www.gnu.org/licenses/

#include "UAVX.h"

// CAUTION: Flash does not follow SPI "convention" where MSB signifies a read operation

// TODO: (IntAdr + len) < buffersize

// Read Ops

//#define RD_CONT			0x0b
#define RD_CONT				0x1b
#define ERASE_BLOCK_FL		0x50
//#define RD_PAGE_FL		0x52
#define RD_PAGE_FL_B1 		0x53
//#define RD_B1_HF			0x54
#define RD_PAGE_FL_B2 		0x55
//#define RD_B2_HF			0x56
//#define STATUS_FL			0x57
#define RD_MOD_WR_B1_FL		0x58
#define RD_MOD_WR_B2_FL		0x59
#define CMP_B1_FL    		0x60
#define CMP_B2_FL	    	0x61

//#define RD_CONT			0x68
#define ERASE_SECTOR_FL		0x7c

// Write Ops

#define ERASE_PAGE_FL       0x81
#define WR_VIA_B1_FL		0x82
#define WR_B1_FL_E   		0x83
#define WR_B1				0x84
#define WR_VIA_B2_FL		0x85
#define WR_B2_FL_E   		0x86
#define WR_B2				0x87
#define WR_B1_FL     		0x88
#define WR_B2_FL		    0x89

#define DEV_FL				0x9f

#define RD_FL				0xd2
#define RD_B1_HF			0xd4
#define RD_B2_HF			0xd6
#define STATUS_FL			0xd7

uint8 oxo;
uint8 flashInfo[5] = { 0xff };
uint8 flashStatus1, flashStatus2;

boolean flashInit(void) {
	boolean r;

	flashReset(memSel);
	r = flashDeviceInfoValid(memSel);
	r &= flashConfig256(memSel); // MEM_PAGE_SIZE

	return(r);
} // flashInit


void flashShowStatus(uint8 s) {
	uint8 i;

	TxString(s, "0x");
	TxValH(s, flashStatus1);
	TxChar(s, ' ');
	for (i = 0; i < 5; i++) {
		TxString(s, "0x");
		TxValH(s, flashInfo[i]);
		TxChar(s, ' ');
	}

} // flashShowStatus


boolean flashFlagSet(uint8 devSel, uint8 fb) {

	return ((flashReadStatus(devSel) & (1 << fb)) != 0);

} // flashFlagSet


void flashSendAddress(SPI_TypeDef * s, uint32 a) {
	oxo = spiSend(s, (uint8) (a >> 16));
	oxo = spiSend(s, (uint8) (a >> 8));
	oxo = spiSend(s, (uint8) a);
} // flashPageAddress

//___________________________________________________________________

// READ


uint8 flashReadStatus(uint8 devSel) {
	SPI_TypeDef * s;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	s = spiSetBaudRate(devSel, true);

	spiSelect(devSel, true);
	oxo = spiSend(s, STATUS_FL);
	flashStatus1 = spiSend(s, 0);
	//flashStatus2 = spiSend(s, 0);
	spiSelect(devSel, false);

	return(flashStatus1);

} // flashReadStatus

boolean flashDeviceInfoValid(uint8 devSel) {
	const uint8 expflashInfo[5] = { 0x1f, 0x28, 0x00, 0x01, 0x00 };
	uint8 i;
	SPI_TypeDef * s;
	boolean r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	s = spiSetBaudRate(devSel, true);

	spiSelect(devSel, true);
	oxo = spiSend(s, DEV_FL);
	for (i = 0; i < 5; i++)
		flashInfo[i] = spiSend(s, 0);
	spiSelect(devSel, false);

	r = true;
	for (i = 0; i < 5; i++)
		r &= flashInfo[i] == expflashInfo[i];

	return (r);

} // flashDeviceInfoValid


void flashReset(uint8 devSel) {
	SPI_TypeDef * s;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	oxo = spiSend(s, 0x80);
	oxo = spiSend(s, 0);
	oxo = spiSend(s, 0);
	oxo = spiSend(s, 0);
	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 35);

} // flashReset


boolean flashReadPage(uint8 devSel, uint32 a, uint32 len, int8 * data) {
	uint32 i;
	SPI_TypeDef * s;
	uint32 r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	r = spiErrors;

	s = spiSetBaudRate(devSel, true);

	spiSelect(devSel, true);
	oxo = spiSend(s, RD_FL);
	flashSendAddress(s, a);
	oxo = spiSend(s, 0); // dummy to allow setup
	oxo = spiSend(s, 0);
	oxo = spiSend(s, 0);
	oxo = spiSend(s, 0);
	for (i = 0; i < len; i++)
		data[i] = spiSend(s, 0);
	spiSelect(devSel, false);

	return (r == spiErrors);

} // flashReadPage

//____________________________________________________________________________

// WRITE


boolean flashConfig256(uint8 devSel) {
	SPI_TypeDef * s;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	oxo = spiSend(s, 0x3d);
	oxo = spiSend(s, 0x2a);
	oxo = spiSend(s, 0x80);
	oxo = spiSend(s, 0xa6);
	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 35000);

	return(flashFlagSet(devSel, FLAG_256));

} // flashConfig256


boolean flashReadModifyWrite(uint8 devSel, uint32 a, uint32 len, int8 *data) {
	uint32 i;
	SPI_TypeDef * s;
	uint32 r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	r = spiErrors;

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	oxo = spiSend(s, RD_MOD_WR_B1_FL);
	flashSendAddress(s, a);
	for (i = 0; i < len; i++)
		oxo = spiSend(s, data[i]);
	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 35000);

	return (r == spiErrors);

} // flashReadModifyWrite

//____________________________________________________________________________

// ERASE operations

boolean flashErasePage(uint8 devSel, uint32 a) {
	SPI_TypeDef * s;
	uint32 r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	r = spiErrors;

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	oxo = spiSend(s, ERASE_PAGE_FL);
	flashSendAddress(s, a);
	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 35000);

	return (r == spiErrors);

} // flashErasePage

boolean flashEraseSector(uint8 devSel, uint32 a) {
	SPI_TypeDef * s;
	uint32 r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	r = spiErrors;

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);

	oxo = spiSend(s, ERASE_SECTOR_FL);
	flashSendAddress(s, a);

	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 6500000);

	return (r == spiErrors);

} // flashEraseSector


boolean flashErase(uint8 devSel) {
	SPI_TypeDef * s;
	uint32 r;

	while (uSClock() < uS[MemReady]) {
		// BLOCKING
	};

	r = spiErrors;

	s = spiSetBaudRate(devSel, false);

	spiSelect(devSel, true);
	oxo = spiSend(s, 0xc7);
	oxo = spiSend(s, 0x94);
	oxo = spiSend(s, 0x80);
	oxo = spiSend(s, 0x9a);
	spiSelect(devSel, false);

	uSTimer(uSClock(),MemReady, 208000000);

	return (r == spiErrors);

} // flashErase
