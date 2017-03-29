// =================================================================================================
// =                                  UAVX Quadrocopter Controller                                 =
// =                             Copyright (c) 2008 by Prof. Greg Egan                             =
// =                   Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                       http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// =================================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even 
//    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//    General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

// USART routines

#include "uavx.h"

void TxString(const rom uint8 *);
void TxChar(uint8);
void TxValU(uint8);
void TxValS(int8);
void TxNextLine(void);
void TxNibble(uint8);
void TxValH(uint8);
void TxValH16(uint16);
uint8 RxChar(void);
uint8 PollRxChar(void);
uint8 RxChar(void);
uint8 RxNumU(void);
int8 RxNumS(void);
void TxVal32(int32, int8, uint8);
void TxChar(uint8);
void TxESCu8(uint8);
void Sendi16(int16);
void TxESCi8(int8);
void TxESCi16(int16);
void TxESCi24(int24);
void TxESCi32(int32);
void SendPacket(uint8, uint8, uint8 *, boolean);

#pragma udata txbuffer
uint8 TxCheckSum;
uint8Q TxQ;
#pragma udata

void TxString(const rom uint8 *pch)
{
	while( *pch != '\0' )
		TxChar(*pch++);
} // TxString

void TxChar(uint8 ch)
{
	uint8 NewTail;

	if ( F.TxToBuffer )
	{
		TxCheckSum ^= ch;
		NewTail=(TxQ.Tail+1) & TX_BUFF_MASK;
	  	TxQ.B[NewTail]=ch;
		TxQ.Tail = NewTail;
	}
	else
	{
		while( !PIR1bits.TXIF ) ;	// wait for transmit ready
		TXREG = ch;		// put new char
	}
} // TxChar

void TxValU(uint8 v)
{	
	// UAVPSet requires 3 digits exactly ( 000 to 999 )
	TxChar((v / 100) + '0');
	v %= 100;	

	TxChar((v / 10) + '0');
	v %= 10;

	TxChar(v + '0');
} // TxValU

void TxValS(int8 v)
{
	// UAVPSet requires sign and 3 digits exactly (-999 to 999)
	if( v < 0 )
	{
		TxChar('-');	// send sign
		v = -v;
	}
	else
		TxChar('+');	// send sign

	TxValU(v);
} // TxValS

void TxNextLine(void)
{
	TxChar(CR);
	TxChar(LF);
} // TxNextLine

void TxNibble(uint8 v)
{
	if ( v > (uint8)9)
		TxChar('A' + v - 10);
	else
		TxChar('0' + v);
} // TxNibble

void TxValH(uint8 v)
{
	TxNibble(v >> 4);
	TxNibble(v & 0x0f);
} // TxValH

void TxValH16(uint16 v)
{
	TxValH(v >> 8);
	TxValH(v);
} // TxValH16

uint8 PollRxChar(void)
{
	uint8	ch;	

	if( PIR1bits.RCIF )	// a character is waiting in the buffer
	{
		if( RCSTAbits.OERR || RCSTAbits.FERR )	// overrun or framing error?
		{
			RCSTAbits.CREN = false;	// disable, then re-enable port to
			RCSTAbits.CREN = true;	// reset OERR and FERR bit
			ch = RCREG;	// dummy read
		}
		else
		{
			ch = RCREG;	// get the character
			TxChar(ch);	// echo it for UAVPSet
			return(ch);		// and return it
		}
	}
	return( NUL );	// nothing in buffer

} // PollRxChar

uint8 RxChar(void)
{
	uint8	ch;	

	while ( !PIR1bits.RCIF );

	if( RCSTAbits.OERR || RCSTAbits.FERR )	// overrun or framing error?
	{
		RCSTAbits.CREN = false;	// disable, then re-enable port to
		RCSTAbits.CREN = true;	// reset OERR and FERR bit
		ch = RCREG;	// dummy read
	}
	else
		ch = RCREG;	// get the character
	
	return(ch);
} // RxChar


uint8 RxNumU(void)
{
	// UAVPSet sends 2 digits
	uint8 ch;
	uint8 n;

	n = 0;
	do
		ch = PollRxChar();
	while( (ch < '0') || (ch > '9') );
	n = (ch - '0') * 10;
	do
		ch = PollRxChar();
	while( (ch < '0') || (ch > '9') );
	n += ch - '0';
	return(n);
} // RxNumU


int8 RxNumS(void)
{
	// UAVPSet sends sign and 2 digits
	uint8 ch;
	int8 n;
	boolean Neg;
	n = 0;

	Neg = false;
	do
		ch = PollRxChar();
	while( ((ch < '0') || (ch > '9')) &&
           (ch != '-') );
	if( ch == '-' )
	{
		Neg = true;
		do
			ch = PollRxChar();
		while( (ch < '0') || (ch > '9') );
	}
	n = (ch - '0') * 10;

	do
		ch = PollRxChar();
	while( (ch < '0') || (ch > '9') );
	n += ch - '0';
	if( Neg )
		n = -n;
	return(n);
} // RxNumS

void TxVal32(int32 V, int8 dp, uint8 Separator)
{
	uint8 S[16];
	int8 c, Rem, zeros, i;
	int32 NewV;
	 
	if (V<0)
	{
		TxChar('-');
	    V=-V;
	}
//	else
//		TxChar(' ');
	
	c=0;
	do
	{
	    NewV=V/10;
	    Rem=V-(NewV*10);
	    S[c++]=Rem + '0';
	    V=NewV;
	}
	while (V>0);
	  
	if ((c < ( dp + 1 ) ) && (dp > 0 ))
	{
	    TxChar('0');
	    TxChar('.');
	} 

	zeros = (int8)dp-c-1;
	if ( zeros >= 0 ) 
		for (i = zeros; i>=0; i--)
			TxChar('0');

	do
	{
	    c--;
	    TxChar(S[c]);
	    if ((c==dp)&&(c>0)) 
	      TxChar('.');
	}
	while ( c > 0 );

	if ( Separator != NUL )
		TxChar(Separator);
} // TxVal32

void TxESCu8(uint8 ch)
{
  if ((ch==SOH)||(ch==EOT)||(ch==ESC))
	TxChar(ESC);
  TxChar(ch);
} // TxESCu8

void TxESCi8(int8 b)
{
  if (((uint8)b==SOH)||((uint8)b==EOT)||((uint8)b==ESC))
	TxChar(ESC);
  TxChar(b);
} // TxESCu8

void Sendi16(int16 v)
{
	i16u Temp;

	Temp.i16 = v;
	TxChar(Temp.b0);
	TxChar(Temp.b1);
} // Sendi16

void TxESCi16(int16 v)
{
	i16u Temp;

	Temp.i16 = v;
	TxESCu8(Temp.b0);
	TxESCu8(Temp.b1);
} // Sendi16

void TxESCi24(int24 v)
{
	i24u Temp;

	Temp.i24 = v;
	TxESCu8(Temp.b0);
	TxESCu8(Temp.b1);
	TxESCu8(Temp.b2);
} // Sendi16

void TxESCi32(int32 v)
{
	i32u Temp;

	Temp.i32 = v;
	TxESCi16(Temp.w0);
	TxESCi16(Temp.w1);
} // TxESCi32

