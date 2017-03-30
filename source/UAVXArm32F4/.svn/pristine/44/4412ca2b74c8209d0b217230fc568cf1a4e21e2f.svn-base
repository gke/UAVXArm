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

boolean sioReadBlock(uint8 sioDev, uint8 id, uint8 reg, uint8 len, uint8 * data) {
	if (spiDevUsed[sioDev])
		return (spiReadBlock(sioDev, id, reg, len, data));
	else
		return (i2cReadBlock(sioDev, id, reg, len, data));

} // sioReadBlock


boolean sioWriteBlock(uint8 sioDev, uint8 id, uint8 reg, uint8 len, uint8 * data) {
	if (spiDevUsed[sioDev])
		return (spiWriteBlock(sioDev, id, reg, len, data));
	else
		return (i2cWriteBlock(sioDev, id, reg, len, data));

} // sioWriteBlock


// derivative


uint8 sioRead(uint8 sioDev, uint8 id, uint8 reg) {
	uint8 v;

	sioReadBlock(sioDev, id, reg, 1, &v);
	return(v);
} // i2cRead

boolean sioWrite(uint8 sioDev, uint8 id, uint8 reg, uint8 data) {

	return (sioWriteBlock(sioDev, id, reg, 1, &data));
} // sioWrite


uint8 sioReadataddr(uint8 sioDev, uint8 d, uint8 a) {
	uint8 v;

	sioReadBlock(sioDev, d, a, 1, &v);

	return (v);
} // readataddr


boolean sioReadBlockataddr(uint8 sioDev, uint8 d, uint8 a, uint8 l, uint8 *S) {
	return (sioReadBlock(sioDev, d & 0xfe, a, l, S));
} // blockreadataddr

boolean sioReadBlocki16v(uint8 sioDev, uint8 d, uint8 l, int16 *v, boolean h) {
	uint8 ll, b;
	uint8 S[32];
	boolean r;

	ll = l * 2;
	r = sioReadBlock(sioDev, d, 0, ll, S);

	if (h) {
		for (b = 0; b < ll; b += 2)
			v[b >> 1] = ((int16) S[b] << 8) | S[b + 1];
	} else {
		for (b = 0; b < ll; b += 2)
			v[b >> 1] = ((int16) S[b + 1] << 8) | S[b];
	}

	return (r);
} // readi16v

boolean sioReadBlocki16vataddr(uint8 sioDev, uint8 d, uint8 a, uint8 l,
		int16 *v, boolean h) {
	uint8 b, ll;
	uint8 S[32];

	ll = l * 2;
	sioReadBlockataddr(sioDev, d, a, ll, S);

	if (h) {
		for (b = 0; b < ll; b += 2)
			v[b >> 1] = ((int16) S[b] << 8) | S[b + 1];
	} else {
		for (b = 0; b < ll; b += 2)
			v[b >> 1] = ((int16) S[b + 1] << 8) | S[b];
	}

	return (true);
} // readi16v

boolean sioWritebyte(uint8 sioDev, uint8 d, uint8 v) {

	return (sioWrite(sioDev, d, 0, v));
} // writeataddr

boolean sioWriteataddr(uint8 sioDev, uint8 d, uint8 a, uint8 v) {

	return (sioWrite(sioDev, d, a, v));
} // writeataddr

/* zzz
boolean sioWriteBlock(uint8 sioDev, uint8 d, uint8 l, uint8 * data) {
	uint8 i;
	boolean r = true;

	for (i = 0; i < l; i++)
		r &= sioWrite(sioDev, d, 0, data++);

	return (r);
} // blockwrite
*/


boolean sioWriteBlockataddr(uint8 sioDev, uint8 d, uint8 a, uint8 l, uint8 *S) {
	uint8 i;
	boolean r = true;

	for (i = 0; i < l; i++)
		r &= sioWrite(sioDev, d, a++, *S++);

	return (r);
} // sioWriteBlockataddr

boolean sioResponse(uint8 sioDev, uint8 d) {
	// returns true unless there is an I2C timeout????
	uint8 v;

	v = 77;
	return (sioReadBlock(sioDev, d, 0, 1, &v) && (v != 77));

} // response


