/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 * Author: 4712
 */

#include "UAVX.h"

#define USE_TXRX_LED (MAX_LEDS>0)
#if defined(USE_TXRX_LED)
# define RX_LED_OFF   LEDOff(LEDBlueSel)
# define RX_LED_ON    LEDOn(LEDBlueSel)

# define TX_LED_OFF   LEDOff(LEDYellowSel)
# define TX_LED_ON    LEDOn(LEDYellowSel)

#define LED0_OFF		LEDOff(LEDRedSel)
#define LED0_ON			LEDOn(LEDRedSel)

#define LED1_OFF		LEDOff(LEDGreenSel)
#define LED1_ON			LEDOn(LEDGreenSel)

#else
# define RX_LED_OFF   do {} while(0)
# define RX_LED_ON    do {} while(0)
# define TX_LED_OFF   do {} while(0)
# define TX_LED_ON    do {} while(0)
#endif

#define SERIAL_4WAY_INTERFACE_NAME_STR "m4wFCIntf"
#define SERIAL_4WAY_VER_MAIN  14
#define SERIAL_4WAY_VER_SUB_1 4
#define SERIAL_4WAY_VER_SUB_2 4
#define SERIAL_4WAY_PROTOCOL_VER 106

#if SERIAL_4WAY_VER_MAIN > 24
# error "SERIAL_4WAY_VER_MAIN * 10 + SERIAL_4WAY_VER_SUB_1 must fit in uint8_t"
#endif
#if SERIAL_4WAY_VER_SUB_1 >= 10
# warning "SERIAL_4WAY_VER_SUB_1 should be 0-9"
#endif

#if SERIAL_4WAY_VER_SUB_2 >= 100
# warning "SERIAL_4WAY_VER_SUB_2 should be <= 99 (9.9)"
#endif

#define SERIAL_4WAY_VERSION_HI (uint8_t)(SERIAL_4WAY_VER_MAIN * 10 + SERIAL_4WAY_VER_SUB_1)
#define SERIAL_4WAY_VERSION_LO (uint8_t)(SERIAL_4WAY_VER_SUB_2)

static uint8_t escCount;
uint8_t escSelected;

boolean BLHelSuiteActive = false;
boolean esc4wayExitRequested = false;

static escDeviceInfo_t deviceInfo;

static boolean isMcuConnected(void) {
	return deviceInfo.signature != 0;
}

static void setDisconnected(void) {
	deviceInfo.signature = 0;
}

__attribute__((always_inline))                                   inline boolean isEscHi(uint8_t selEsc) {
	return digitalRead(&PWMPins[selEsc]);
}

__attribute__((always_inline))                                   inline boolean isEscLo(uint8_t selEsc) {
	return !digitalRead(&PWMPins[selEsc]);
}

__attribute__((always_inline)) inline void setEscHi(uint8_t selEsc) {
	digitalWrite(&PWMPins[selEsc], 1);
}

__attribute__((always_inline)) inline void setEscLo(uint8_t selEsc) {
	digitalWrite(&PWMPins[selEsc], 0);
}

void setEscInput(uint8_t selEsc) {
	pinInitMode(&PWMPins[selEsc], true);
}

void setEscOutput(uint8_t selEsc) {
	pinInitMode(&PWMPins[selEsc], false);
	//pinInitOutput(&PWMPins[selEsc]);
}

void StopPwmAllMotors(void) {
	uint8_t i;

	for (i = 0; i < NoOfDrives; i++)
		*PWMPins[i].Timer.CCR = 0;

	Delay1mS(50); // wait for timers to settle
}

// Initialise 4way ESC interface
// initialises internal structures
// returns number of ESCs available
int esc4wayInit(void) {
	uint8_t i;

	StopPwmAllMotors();

	for (i = 0; i < NoOfDrives; i++) {// switch off all (potential) motor output pins
		pinInitMode(&PWMPins[i], true); // TODO: original code uses 2MHz pin clock?
		setEscHi(i);
	}

	escCount = NoOfDrives; // escIdx;
	return escCount;
}

// start BLHeli 4way interface
// sets all ESC lines as output + hi
void esc4wayStart(void) {

	//pwmDisableMotors();    // prevent updating PWM registers
	for (int i = 0; i < NoOfDrives; i++) {
		setEscInput(i);
		setEscHi(i);
	}
}

// stops BLHeli 4way interface
// returns all claimed pins back to PWM drivers, re-enables PWM
void esc4wayRelease(void) {

	for (int i = 0; i < NoOfDrives; i++) {
		setEscOutput(i);
		setEscLo(i);
	}

	Delay1mS(5000);

	escCount = 0;
	InitDrives();
}

// BLHeliSuite packet framing
// for reference, see 'Manuals/BLHeliSuite 4w-if protocol.pdf' from BLHeliSuite
// Send Structure
// ESC CMD ADDR_H ADDR_L PARAM_LEN PARAM (256B if len == 0) CRC16_Hi CRC16_Lo
// Return
// ESC CMD ADDR_H ADDR_L PARAM_LEN PARAM (256B if len == 0) + ACK (uint8_t OK or ERR) + CRC16_Hi CRC16_Lo

// esc4wayCmd_e in public header

typedef enum {
	// not commands, but keep naming consistent with BLHeli suite
	cmd_Remote_Escape = 0x2E, // '.'
	cmd_Local_Escape = 0x2F,
// '/'
} syn_4way_e;

/*
 Copyright (c) 2002, 2003, 2004  Marek Michalkiewicz
 Copyright (c) 2005, 2007 Joerg Wunsch
 Copyright (c) 2013 Dave Hylands
 Copyright (c) 2013 Frederic Nadeau
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.

 * Neither the name of the copyright holders nor the names of
 contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE. */

uint16_t _crc_xmodem_update(uint16_t crc, uint8_t data) {
	int i;

	crc = crc ^ ((uint16_t) data << 8);
	for (i = 0; i < 8; i++) {
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	}
	return crc;
}
// * End copyright

static uint16_t signaturesAtmel[] = { 0x9307, 0x930A, 0x930F, 0x940B, 0 };
static uint16_t signaturesSilabs[] = { 0xF310, 0xF330, 0xF410, 0xF390, 0xF850,
		0xE8B1, 0xE8B2, 0 };

boolean signatureMatch(uint16_t signature, uint16_t *list) {
	for (; *list; list++)
		if (signature == *list)
			return true;
	return false;
}

static uint8_t currentInterfaceMode;

// Try connecting to device
// 3 attempts are made, trying both STK and BL protocols.
uint8_t connect(escDeviceInfo_t *pDeviceInfo) {
	for (int try = 0; try < 3; try++) {
		if (Stk_ConnectEx(pDeviceInfo) && signatureMatch(
				pDeviceInfo->signature, signaturesAtmel)) {
			currentInterfaceMode = imSK;
			return 1;
		}
		if (BL_ConnectEx(pDeviceInfo)) {
			if (signatureMatch(pDeviceInfo->signature, signaturesSilabs)) {
				currentInterfaceMode = imSIL_BLB;
				return 1;
			}
			if (signatureMatch(pDeviceInfo->signature, signaturesAtmel)) {
				currentInterfaceMode = imATM_BLB;
				return 1;
			}
		}
	}
	return 0;
}

static uint8_t port;
static uint16_t crcIn, crcOut;

uint8_t readByte(void) {
	// need timeout?
	while (!serialAvailable(port))
		;
	return RxChar(port);
}

uint8_t readByteCrc(void) {
	uint8_t b = readByte();
	crcIn = _crc_xmodem_update(crcIn, b);
	return b;
}

void writeByte(uint8_t b) {
	TxBin8(port, b);
}

void writeByteCrc(uint8_t b) {
	writeByte(b);
	crcOut = _crc_xmodem_update(crcOut, b);
}

// handle 4way interface on serial port
// esc4wayStart / esc4wayRelease in called internally
// 256 bytes buffer is allocated on stack
void esc4wayProcess(uint8 s) {
	uint8_t command;
	uint16_t addr;
	int inLen;
	int outLen;
	uint8_t paramBuf[256];
	uint8_t replyAck;
	uint8_t esc;

	port = s;

	LEDsOff();
	BeeperOff();

	esc4wayStart();

	esc4wayExitRequested = false;
	while (!esc4wayExitRequested) {

		if (serialAvailable(s)) {

			crcIn = 0;
			esc = readByteCrc();
			if (esc == cmd_Local_Escape) { //assume rest will follow quickly?
				LEDOn(LEDYellowSel);

				command = readByteCrc();
				addr = readByteCrc() << 8;
				addr |= readByteCrc();

				inLen = readByteCrc();
				if (inLen == 0)
					inLen = 0x100; // len ==0 -> param is 256B

				for (int i = 0; i < inLen; i++)
					paramBuf[i] = readByteCrc();

				readByteCrc();
				readByteCrc(); // update input CRC

				outLen = 0; // output handling code will send single zero byte if necessary

				replyAck = crcIn == 0 ? esc4wayProcessCmd(command, addr,
						paramBuf, inLen, &outLen) : esc4wayAck_I_INVALID_CRC;

				// send single '\0' byte is output when length is zero (len ==0 -> 256 bytes)
				if (outLen == 0) {
					paramBuf[0] = 0;
					outLen = 1;
				}

				crcOut = 0;

				writeByteCrc(cmd_Remote_Escape);
				writeByteCrc(command);
				writeByteCrc(addr >> 8);
				writeByteCrc(addr & 0xff);
				writeByteCrc(outLen & 0xff); // only low byte is send, 0x00 -> 256B
				for (int i = 0; i < outLen; i++)
					writeByteCrc(paramBuf[i]);
				writeByteCrc(replyAck);
				writeByte(crcOut >> 8);
				writeByte(crcOut & 0xff);

				LEDOff(LEDYellowSel);

			}
		}
	}

	esc4wayRelease();
}

// handle 4Way interface command
// command - received command, will be sent back in reply
// addr - from received header
// data - buffer used both for received parameters and returned data.
//   Should be 256B long ; TODO - implement limited buffer size
// inLen - received input length
// outLen - size of data to return, max 256, initialised to zero
//          single '\0' byte will be send if outLen is zero (protocol limitation)
esc4wayAck_e esc4wayProcessCmd(esc4wayCmd_e command, uint16_t addr,
		uint8_t *data, int inLen, int *outLen) {
	ioMem_t ioMem;
	ioMem.addr = addr; // default flash operation address
	ioMem.data = data; // command data buffer is used for read and write commands

	switch (command) {
	// ******* Interface related stuff *******
	case cmd_InterfaceTestAlive:
		if (!isMcuConnected())
			return esc4wayAck_OK;

		switch (currentInterfaceMode) {
		case imATM_BLB:
		case imSIL_BLB:
			if (BL_SendCMDKeepAlive())
				return esc4wayAck_OK;
			break;
		case imSK:
			if (Stk_SignOn())
				return esc4wayAck_OK;
			break;
		}
		setDisconnected();
		return esc4wayAck_D_GENERAL_ERROR;
	case cmd_ProtocolGetVersion:
		// Only interface itself, no matter what Device
		data[0] = SERIAL_4WAY_PROTOCOL_VER;
		*outLen = 1;
		return esc4wayAck_OK;
	case cmd_InterfaceGetName:
		// Only interface itself, no matter what Device
		// outLen=16;
		memcpy(data, SERIAL_4WAY_INTERFACE_NAME_STR, strlen(
				SERIAL_4WAY_INTERFACE_NAME_STR));
		*outLen = strlen(SERIAL_4WAY_INTERFACE_NAME_STR);
		return esc4wayAck_OK;
	case cmd_InterfaceGetVersion:
		// Only interface itself, no matter what Device
		data[0] = SERIAL_4WAY_VERSION_HI;
		data[1] = SERIAL_4WAY_VERSION_LO;
		*outLen = 2;
		return esc4wayAck_OK;
	case cmd_InterfaceExit: // does this first
		esc4wayExitRequested = true;
		return esc4wayAck_OK;
	case cmd_InterfaceSetMode:
		switch (data[0]) {
		case imSIL_BLB:
		case imATM_BLB:
		case imSK:
			currentInterfaceMode = data[0];
			break;
		default:
			return esc4wayAck_I_INVALID_PARAM;
		}
		return esc4wayAck_OK;
	case cmd_DeviceReset:
		if (data[0] >= escCount)
			return esc4wayAck_I_INVALID_CHANNEL;
		// Channel may change here
		escSelected = data[0];
		switch (currentInterfaceMode) {
		case imSIL_BLB:
		case imATM_BLB:
			BL_SendCMDRunRestartBootloader();
			break;
		case imSK:
			break;
		}
		setDisconnected();
		return esc4wayAck_OK;
	case cmd_DeviceInitFlash:
		setDisconnected();
		if (data[0] >= escCount)
			return esc4wayAck_I_INVALID_CHANNEL;
		//Channel may change here
		//ESC_LO or ESC_HI; Halt state for prev channel
		int replyAck = esc4wayAck_OK;
		escSelected = data[0];
		if (!connect(&deviceInfo)) {
			setDisconnected();
			return esc4wayAck_D_GENERAL_ERROR;
		}
		deviceInfo.interfaceMode = currentInterfaceMode;
		memcpy(data, &deviceInfo, sizeof(deviceInfo));
		*outLen = sizeof(deviceInfo);
		return replyAck;
	case cmd_DeviceEraseAll:
		switch (currentInterfaceMode) {
		case imSK:
			if (!Stk_Chip_Erase())
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		default:
			return esc4wayAck_I_INVALID_CMD;
		}
		return esc4wayAck_OK;
	case cmd_DevicePageErase:
		switch (currentInterfaceMode) {
		case imSIL_BLB:
			*outLen = 1; // return block number (from incoming packet)
			// Address = Page * 512
			ioMem.addr = data[0] << 9;
			if (!BL_PageErase(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		default:
			return esc4wayAck_I_INVALID_CMD;
		}
		return esc4wayAck_OK;

		//*** Device Memory Read Ops ***

		// macros to mix interface with (single bit) memory type for switch statement
#define M_FLASH 0
#define M_EEPROM 1
#define INTFMEM(interface, memory) (((interface) << 1) | (memory))

	case cmd_DeviceReadEEprom:
	case cmd_DeviceRead: {
		int len = data[0];
		if (len == 0)
			len = 0x100;
		ioMem.len = len;
		switch (INTFMEM(currentInterfaceMode, (command == cmd_DeviceRead) ? M_FLASH : M_EEPROM)) {
		case INTFMEM(imSIL_BLB, M_FLASH):
			if (!BL_ReadFlashSIL(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imATM_BLB, M_FLASH):
			if (!BL_ReadFlashATM(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imATM_BLB, M_EEPROM):
			if (!BL_ReadEEprom(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
			// INTFMEM(imSIL_BLB, M_EEPROM): no eeprom on Silabs
		case INTFMEM(imSK, M_FLASH):
			if (!Stk_ReadFlash(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imSK, M_EEPROM):
			if (!Stk_ReadEEprom(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		default:
			return esc4wayAck_I_INVALID_CMD;
		}
		*outLen = ioMem.len;
		return esc4wayAck_OK;
	}

		//*** Device Memory Write Ops ***
	case cmd_DeviceWrite:
	case cmd_DeviceWriteEEprom:
		ioMem.len = inLen;
		switch (INTFMEM(currentInterfaceMode, (command == cmd_DeviceWrite) ? M_FLASH : M_EEPROM)) {
		case INTFMEM(imSIL_BLB, M_FLASH):
		case INTFMEM(imATM_BLB, M_FLASH):
			if (!BL_WriteFlash(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imATM_BLB, M_EEPROM):
			if (!BL_WriteEEprom(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imSK, M_FLASH):
			if (!Stk_WriteFlash(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		case INTFMEM(imSK, M_EEPROM):
			if (!Stk_WriteEEprom(&ioMem))
				return esc4wayAck_D_GENERAL_ERROR;
			break;
		default:
			return esc4wayAck_I_INVALID_CMD;
		}
		return esc4wayAck_OK;
#undef M_FLASH
#undef M_EEPROM
#undef INTFMEM
	default:
		return esc4wayAck_I_INVALID_CMD;
	}
	// should not get here
}

uint8_t reqCmd;

enum {
	RxSentinel, RxHeader, RxDirection, RxLength, RxCmd, RxData, RxCSum, RxIdle
};

uint8_t mspRxState = RxSentinel;
uint8_t mspData[64];

boolean GetMSPPacket(uint8_t s) {
	static uint8_t mspCSum = 0;
	uint8_t len = 0;
	uint8_t offset = 0;
	uint8_t c;

	if (serialAvailable(s)) {
		c = RxChar(s);
		mspCSum ^= c;
		switch (mspRxState) {
		case RxSentinel:
			if (c == '$')
				mspRxState = RxHeader;
			break;
		case RxHeader:
			if (c == 'M')
				mspRxState = RxDirection;
			break;
		case RxDirection:
			if (c == '<')
				mspRxState = RxLength;
			break;
		case RxLength:
			mspCSum = len = c;
			offset = 0;
			mspRxState = RxCmd;
			break;
		case RxCmd:
			reqCmd = c;
			mspRxState = (len > 0) ? RxData : RxCSum;
			break;
		case RxData:
			if (offset >= 64)
				mspRxState = RxSentinel;
			else if (offset < len)
				mspData[++offset] = c;
			else
				mspRxState = RxCSum;
			break;
		case RxCSum:
			mspRxState = RxSentinel;
			if (mspCSum == 0)
				return true;
			break;
		default:
			break;
		}
	}
	return false;
}

void mspHeader(uint8_t s, uint8_t len, uint8_t reqCmd) {
	TxChar(s, '$');
	TxChar(s, 'M');
	TxChar(s, '>');
	TxCheckSum[s] = 0;
	TxBin8(s, len);
	TxBin8(s, reqCmd);
}

void mspFooter(uint8_t s) {
	TxBin8(s, TxCheckSum[s]);
}

boolean DoMSPCmds(uint8_t s) {
	boolean mspContinue = true;

#define MSP_PROTOCOL_VERSION	0

#define API_VERSION_MAJOR		1
#define API_VERSION_MINOR		25
#define API_VERSION_LENGTH		2

#define MW_VERSION 231

#define FC_VERSION_MAJOR		1
#define FC_VERSION_MINOR		14
#define FC_VERSION_PATCH_LEVEL	1
	// MSP labels
#define MSP_API_VERSION 		1 //out message
#define MSP_FC_VARIANT			2 //out message
#define MSP_FC_VERSION			3 //out message
#define MSP_BOARD_INFO			4 //out message
#define MSP_BUILD_INFO			5 //out message
#define MSP_NAME				10 //out message          Returns user set board name - betaflight
#define MSP_SET_NAME			11 //in message           Sets board name - betaflight
#define MSP_FEATURE 			36
	//#define MSP_SET_FEATURE		37
#define MSP_DATAFLASH_SUMMARY   70 //out message - get description of dataflash chip
#define MSP_REBOOT				68 //in message reboot settings
#define MSP_IDENT				100 //out message mixer Mode
#define MSP_STATUS				101    //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RAW_IMU				102    //out message         9 DOF
#define MSP_SERVO				103    //out message         servos
#define MSP_MOTOR				104    //out message         motors
#define MSP_MISC				114    //out message         powermeter trig
#define MSP_3D					124    //out message         Settings needed for reversible ESCs
	//
#define MSP_SET_1WIRE  			243
#define MSP_SET_4WAY_IF			245    //in message Sets 4way interface
	// Names
#define GIT_SHORT_REVISION_LENGTH   7 // lower case hexadecimal digits.
#define BUILD_DATE_LENGTH 11
	// "MMM DD YYYY" MMM = Jan/Feb/...
#define BUILD_TIME_LENGTH 8
	// "HH:MM:SS"

	do {
		if (GetMSPPacket(s)) {
			LEDOn(LEDBlueSel);
			switch (reqCmd) {
			case MSP_API_VERSION: // 1
				mspHeader(s, 3, MSP_API_VERSION);
				TxBin8(s, MSP_PROTOCOL_VERSION);
				TxBin8(s, API_VERSION_MAJOR);
				TxBin8(s, API_VERSION_MINOR);
				mspFooter(s);
				break;
			case MSP_FC_VARIANT: // 2
				mspHeader(s, 4, MSP_FC_VARIANT);
				TxString(s, "CLFL"); // can only use "known" codes!
				mspFooter(s);
				break;
			case MSP_FC_VERSION:
				mspHeader(s, 3, MSP_FC_VERSION);
				TxBin8(s, FC_VERSION_MAJOR);
				TxBin8(s, FC_VERSION_MINOR);
				TxBin8(s, FC_VERSION_PATCH_LEVEL);
				mspFooter(s);
				break;
			case MSP_BOARD_INFO:
				mspHeader(s, 6, MSP_BOARD_INFO);
				TxString(s, "SPKY");
				TxBin8(s, 0); // No hardware revision available.
				TxBin8(s, 0); // 0 == FC, 1 == OSD, 2 == FC with OSD
				mspFooter(s);
				break;
			case MSP_BUILD_INFO:
				mspHeader(s, 26, MSP_BUILD_INFO);
				TxString(s, BLDate);
				TxString(s, BLTime);
#if defined(V4_BOARD)
				TxString(s, "V4.");
#else
				TxString(s, "V3.");
#endif
				int ll = 7 - strlen(BLRev) - 3;
				TxString(s, BLRev);
				if (ll > 0)
					for (int l = 0; l < ll; l++)
						TxBin8(s, ' ');
				mspFooter(s);
				break;
			case MSP_FEATURE:
				mspHeader(s, 4, MSP_FEATURE);
				TxBin32(s, 0); // u32 feature mask
				mspFooter(s);
				break;
			case MSP_STATUS:
				mspHeader(s, 15, MSP_STATUS);
				TxBin16(s, 500); // TODO:
				TxBin16(s, 0);
				TxBin16(s, 0);
				TxBin32(s, 0); //packFlightModeFlags());
				TxBin8(s, 0);
				TxBin16(s, 0);
				TxBin16(s, 500); // TODO:
				mspFooter(s);
				break;
			case MSP_3D:
				mspHeader(s, 6, MSP_3D);
				TxBin16(s, 0); //motor3DConfig()->deadband3d_low);
				TxBin16(s, 0); //motor3DConfig()->deadband3d_high);
				TxBin16(s, 0); //motor3DConfig()->neutral3d);
				mspFooter(s);
				break;
			case MSP_MISC:
				mspHeader(s, 18, MSP_MISC);
				TxBin16(s, 1500); //rxConfig()->midrc);
				TxBin16(s, 1000); //motorConfig()->minthrottle);
				TxBin16(s, 2000); //motorConfig()->maxthrottle);
				TxBin16(s, 1000); //motorConfig()->mincommand);
				TxBin16(s, 1000); //failsafeConfig()->failsafe_throttle);
				TxBin8(s, 0); //gpsConfig()->provider); // gps_type
				TxBin8(s, 0); // TODO gps_baudrate (an index, cleanflight uses a uint32_t
				TxBin8(s, 0); //gpsConfig()->sbasMode); // gps_ubx_sbas
				TxBin8(s, 0); //mspServerConfig()->multiwiiCurrentMeterOutput);
				TxBin8(s, 0); //rxConfig()->rssi_channel);
				TxBin8(s, 0);
				TxBin16(s, 0); //compassConfig()->mag_declination);
				mspFooter(s);
				break;
			case MSP_MOTOR:
				mspHeader(s, 16, MSP_MOTOR);
				for (int i = 0; i < 8; i++)
					TxBin16(s, i < NoOfDrives ? PW[i] : 0); // TODO: CurrMax?
				mspFooter(s);
				break;
			case MSP_SET_4WAY_IF:
				mspHeader(s, 1, MSP_SET_4WAY_IF);
				//uint8_t e = data[0];
				TxBin8(s, escCount);
				mspFooter(s);
				mspContinue = false;
				break;

				/*
				 case MSP_IDENT:
				 mspHeader(s, 7, MSP_IDENT);
				 TxBin8(s, MW_VERSION);
				 TxBin8(s, 0); // mixerConfig()->mixerMode);
				 TxBin8(s, MSP_PROTOCOL_VERSION);
				 TxBin32(s, 0); // u32 CAP_DYNBALANCE
				 mspFooter(s);
				 break;
				 case MSP_SET_1WIRE:
				 mspHeader(s, 1, MSP_SET_1WIRE);
				 uint8_t e = data[0];
				 TxBin8(s, esc4wayInit()); // no of escs
				 //mspPostProcessFn = msp4WayIfFn;
				 mspFooter(s);
				 break;

				 case MSP_REBOOT:
				 mspHeader(s, 0, MSP_REBOOT);
				 //mspPostProcessFn = mspRebootFn;
				 mspFooter(s);
				 break;
				 case MSP_DATAFLASH_SUMMARY:
				 mspHeader(s, 13, MSP_DATAFLASH_SUMMARY);
				 TxBin8(s, 0); // FlashFS is neither ready nor supported
				 TxBin32(s, 0);
				 TxBin32(s, 0);
				 TxBin32(s, 0);
				 mspFooter(s);
				 break;
				 */

			default:
				break;
			} // switch
			LEDOff(LEDBlueSel);
		}
	} while (mspContinue);

	return mspContinue;
}

void DoBLHeliSuite(uint8_t s) {

	uint32_t Timeout = mSClock() + 5000;

	LEDOn(LEDYellowSel);

	while ((mSClock() < Timeout) && !serialAvailable(s)) {
	};

	if (mSClock() < Timeout) {

		BLHeliSuiteActive = true;

		escCount = esc4wayInit();

		DoMSPCmds(s);

		esc4wayProcess(s);

		esc4wayRelease();

		BLHeliSuiteActive = false;

	}

	LEDsOff();
}

