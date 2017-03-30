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

__attribute__((__section__(".scratchflash"))) const int8 FlashNV[NV_FLASH_SIZE];
#define FLASH_SCRATCH_ADDR (0x8000000+NV_FLASH_SIZE)
#define FLASH_SCRATCH_SECTOR	FLASH_Sector_1 // 11


void ReadBlockNV(uint32 a, uint16 l, int8 * v) {
	uint16 i;

	for (i = 0; i < l; i++)
		v[i] = *(int8 *) (FLASH_SCRATCH_ADDR + a + i);

} // ReadBlockNV


boolean UpdateNV(void) {
	uint16 i;
	boolean r = true;

	//for (i = 0; i < l; i++) // TODO: optimise to word compares
	//	r &= v[i] == FlashNV[a + i]; //*(int8 *) (FLASH_SCRATCH_ADDR + a + i);
	r = false;

	if (!r) {
		FLASH_Unlock();
#if defined(STM32F1)
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		r = FLASH_ErasePage(FLASH_SCRATCH_ADDR) == FLASH_COMPLETE;
#else
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
				| FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
		r = FLASH_EraseSector(FLASH_SCRATCH_SECTOR, VoltageRange_3)
				== FLASH_COMPLETE;
#endif
		if (r) {
			for (i = 0; i < sizeof(NV); i += 4) {
				r = FLASH_ProgramWord(FLASH_SCRATCH_ADDR + i,
						*(uint32 *) ((uint8 *) &NV + i)) == FLASH_COMPLETE;
				if (!r)
					break;
			}
		}
		FLASH_Lock();
	}

	return (r);
} // UpdateNV





