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

#define I2C_DEFAULT_TIMEOUT 30000

#if (MAX_I2C_PORTS>0)

volatile i2cStateDef i2cState[MAX_I2C_PORTS] = { { 0 } };


void i2c_er_handler(uint8 i2cCurr) {
	// Original source unknown but modified from those on baseflight by TimeCop
	volatile uint32 SR1Register, SR2Register;
	I2CPortDef * d;

	d = &I2CPorts[i2cCurr];

	SR1Register = d->I2C->SR1;
	if (SR1Register & 0x0f00) { //an error
		// I2C1error.error = ((SR1Register & 0x0f00) >> 8);        //save error
		// I2C1error.job = job;    //the task
	}
	/* If AF, BERR or ARLO, abandon the current job and commence new if there are jobs */
	if (SR1Register & 0x0700) {
		SR2Register = d->I2C->SR2; //read second status register to clear ADDR if it is set (note that BTF will not be set after a NACK)
		I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //disable the RXNE/TXE interrupt - prevent the ISR tail-chaining onto the ER (hopefully)
		if (!(SR1Register & 0x0200) && !(d->I2C->CR1 & 0x0200)) { //if we don't have an ARLO error, ensure sending of a stop
			if (d->I2C->CR1 & 0x0100) { //We are currently trying to send a start, this is very bad as start,stop will hang the peripheral
				while (d->I2C->CR1 & 0x0100) {//wait for any start to finish sending
				}
				I2C_GenerateSTOP(d->I2C, ENABLE); //send stop to finalise bus transaction
				while (d->I2C->CR1 & 0x0200) {//wait for stop to finish sending
				}
				i2cInit(i2cCurr); //reset and configure the hardware
			} else {
				I2C_GenerateSTOP(d->I2C, ENABLE); //stop to free up the bus
				I2C_ITConfig(d->I2C, I2C_IT_EVT | I2C_IT_ERR, DISABLE); //Disable EVT and ERR interrupts while bus inactive
			}
		}
	}
	d->I2C->SR1 &= ~0x0f00; //reset all the error bits to clear the interrupt
	i2cState[i2cCurr].busy = false;
} // i2c_er_handler

void i2c_ev_handler(uint8 i2cCurr) {
	// Original source unknown but based on those in baseflight by TimeCop
	static int8 index; //index is signed -1==send the sub-address
	uint8 SReg_1; //read the status register here
	I2CPortDef * d;

	d = &I2CPorts[i2cCurr];

	SReg_1 = d->I2C->SR1;

	if (SReg_1 & 0x0001) { //we just sent a start - EV5 in reference manual
		d->I2C->CR1 &= ~0x0800; //reset the POS bit so ACK/NACK applied to the current byte
		I2C_AcknowledgeConfig(d->I2C, ENABLE); //make sure ACK is on
		index = 0; //reset the index
		if (i2cState[i2cCurr].reading && (i2cState[i2cCurr].subaddress_sent
				|| (0xff == i2cState[i2cCurr].reg))) { //we have sent the sub-address
			i2cState[i2cCurr].subaddress_sent = true; //make sure this is set in case of no sub-address, so following code runs correctly
			if (i2cState[i2cCurr].bytes == 2)
				d->I2C->CR1 |= 0x0800; //set the POS bit so NACK applied to the final byte in the two byte read
			I2C_Send7bitAddress(d->I2C, i2cState[i2cCurr].addr,
					I2C_Direction_Receiver); //send the address and set hardware mode
		} else { //direction is Tx, or we haven't sent the sub and rep start
			I2C_Send7bitAddress(d->I2C, i2cState[i2cCurr].addr,
					I2C_Direction_Transmitter); //send the address and set hardware mode
			if (i2cState[i2cCurr].reg != 0xff) //0xff as sub-address means it will be ignored, in Tx or Rx mode
				index = -1; //send a sub-address
		}
	} else if (SReg_1 & 0x0002) { //we just sent the address - EV6 in ref manual
		//Read SR1,2 to clear ADDR
		volatile uint8 a;
		__DMB(); // memory fence to control hardware
		if (i2cState[i2cCurr].bytes == 1 && i2cState[i2cCurr].reading
				&& i2cState[i2cCurr].subaddress_sent) { //we are receiving 1 byte - EV6_3
			I2C_AcknowledgeConfig(d->I2C, DISABLE); //turn off ACK
			__DMB();
			a = d->I2C->SR2; //clear ADDR after ACK is turned off
			I2C_GenerateSTOP(d->I2C, ENABLE);
			i2cState[i2cCurr].final_stop = true;
			I2C_ITConfig(d->I2C, I2C_IT_BUF, ENABLE); //allow us to have an EV7
		} else { //EV6 and EV6_1
			a = d->I2C->SR2; //clear the ADDR here
			__DMB();
			if ((i2cState[i2cCurr].bytes == 2) && i2cState[i2cCurr].reading
					&& i2cState[i2cCurr].subaddress_sent) { //rx 2 bytes - EV6_1
				I2C_AcknowledgeConfig(d->I2C, DISABLE); //turn off ACK
				I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //disable TXE to allow the buffer to fill
			} else if ((i2cState[i2cCurr].bytes == 3)
					&& i2cState[i2cCurr].reading
					&& i2cState[i2cCurr].subaddress_sent) //rx 3 bytes
				I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //make sure RXNE disabled so we get a BTF in two bytes time
			else
				//receiving greater than three bytes, sending sub-address, or transmitting
				I2C_ITConfig(d->I2C, I2C_IT_BUF, ENABLE);
		}
	} else if (SReg_1 & 0x004) { //Byte transfer finished - EV7_2, EV7_3 or EV8_2
		i2cState[i2cCurr].final_stop = true;
		if (i2cState[i2cCurr].reading && i2cState[i2cCurr].subaddress_sent) { //EV7_2, EV7_3
			if (i2cState[i2cCurr].bytes > 2) { //EV7_2
				I2C_AcknowledgeConfig(d->I2C, DISABLE); //turn off ACK
				i2cState[i2cCurr].read_p[index++] = I2C_ReceiveData(d->I2C); //read data N-2
				I2C_GenerateSTOP(d->I2C, ENABLE);
				i2cState[i2cCurr].final_stop = true; //required to fix hardware
				i2cState[i2cCurr].read_p[index++] = I2C_ReceiveData(d->I2C); //read data N-1
				I2C_ITConfig(d->I2C, I2C_IT_BUF, ENABLE); //enable TXE to allow the final EV7
			} else { //EV7_3
				if (i2cState[i2cCurr].final_stop)
					I2C_GenerateSTOP(d->I2C, ENABLE);
				else
					I2C_GenerateSTART(d->I2C, ENABLE); //repeated start
				i2cState[i2cCurr].read_p[index++] = I2C_ReceiveData(d->I2C); //read data N-1
				i2cState[i2cCurr].read_p[index++] = I2C_ReceiveData(d->I2C); //read data N
				index++; //to show job completed
			}
		} else { //EV8_2, which may be due to a sub-address sent or a write completion
			if (i2cState[i2cCurr].subaddress_sent
					|| (i2cState[i2cCurr].writing)) {
				if (i2cState[i2cCurr].final_stop)
					I2C_GenerateSTOP(d->I2C, ENABLE);
				else
					I2C_GenerateSTART(d->I2C, ENABLE); //repeated start
				index++; //to show that the job is complete
			} else { //send a sub-address
				I2C_GenerateSTART(d->I2C, ENABLE); //repeated Start
				i2cState[i2cCurr].subaddress_sent = true; //this is set back to zero upon completion of the current task
			}
		}
		//we must wait for the start to clear, otherwise we get constant BTF
		while (d->I2C->CR1 & 0x0100) {
		}
	} else if (SReg_1 & 0x0040) { //Byte received - EV7
		i2cState[i2cCurr].read_p[index++] = I2C_ReceiveData(d->I2C);
		if (i2cState[i2cCurr].bytes == (index + 3))
			I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //disable TXE to allow the buffer to flush so we can get an EV7_2
		if (i2cState[i2cCurr].bytes == index) //We have completed a final EV7
			index++; //to show job is complete
	} else if (SReg_1 & 0x0080) { //Byte transmitted -EV8/EV8_1
		if (index != -1) { //we don't have a sub-address to send
			I2C_SendData(d->I2C, i2cState[i2cCurr].write_p[index++]);
			if (i2cState[i2cCurr].bytes == index) //we have sent all the data
				I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //disable TXE to allow the buffer to flush
		} else {
			index++;
			I2C_SendData(d->I2C, i2cState[i2cCurr].reg); //send the sub-address
			if (i2cState[i2cCurr].reading || (i2cState[i2cCurr].bytes == 0)) //if receiving or sending 0 bytes, flush now
				I2C_ITConfig(d->I2C, I2C_IT_BUF, DISABLE); //disable TXE to allow the buffer to flush
		}
	}
	if (index == i2cState[i2cCurr].bytes + 1) { //we have completed the current job
		//Completion Tasks go here
		//End of completion tasks
		i2cState[i2cCurr].subaddress_sent = false; //reset this here
		// d->I2C->CR1 &= ~0x0800;   //reset the POS bit so NACK applied to the current byte
		if (i2cState[i2cCurr].final_stop) //if there is a final stop and no more jobs, bus is inactive, disable interrupts to prevent BTF
			I2C_ITConfig(d->I2C, I2C_IT_EVT | I2C_IT_ERR, DISABLE); //Disable EVT and ERR interrupts while bus inactive
		i2cState[i2cCurr].busy = false;
	}
} // i2c_ev_handler


boolean i2cReadBlock(uint8 i2cSel, uint8 id, uint8 reg, uint8 len, uint8* buf) {
	// Original source unknown but based on those in baseflight by TimeCop
	uint32 timeout = I2C_DEFAULT_TIMEOUT;
	uint8 i2cCurr;
	I2CPortDef * d;

	i2cCurr = i2cMap[i2cSel] - 1;
	d = &I2CPorts[i2cCurr];

	i2cState[i2cCurr].addr = id;
	i2cState[i2cCurr].reg = reg;
	i2cState[i2cCurr].writing = false;
	i2cState[i2cCurr].reading = true;
	i2cState[i2cCurr].subaddress_sent = false;
	i2cState[i2cCurr].final_stop = false;
	i2cState[i2cCurr].read_p = buf;
	i2cState[i2cCurr].write_p = buf;
	i2cState[i2cCurr].bytes = len;
	i2cState[i2cCurr].busy = true;

	if (!(d->I2C->CR2 & I2C_IT_EVT)) { //if we are restarting the driver
		if (!(d->I2C->CR1 & 0x0100)) { // ensure sending a start
			while (d->I2C->CR1 & 0x0200) { //wait for any stop to finish sending
			}
			I2C_GenerateSTART(d->I2C, ENABLE); //send the start for the new job
		}
		I2C_ITConfig(d->I2C, I2C_IT_EVT | I2C_IT_ERR, ENABLE); //allow the interrupts to fire off again
	}

	while (i2cState[i2cCurr].busy && (--timeout > 0)) {
	}
	if (timeout == 0) {
		i2cState[i2cCurr].i2cErrors++;
		NV.Stats[I2CFailS] = i2cState[i2cCurr].i2cErrors;
		i2cInit(i2cCurr);
		return (false);
	}

	return (true);
} // i2cReadBlock

boolean i2cWriteBlock(uint8 i2cSel, uint8 id, uint8 reg, uint8 len_,
		uint8 *data) {
	// Original source unknown but based on those in baseflight by TimeCop
	uint8 i, my_data[128]; // TODO: magic number
	uint32 timeout = I2C_DEFAULT_TIMEOUT;
	uint8 i2cCurr;
	I2CPortDef * d;

	if (len_ > 127)
		return (false);

	i2cCurr = i2cMap[i2cSel] - 1;
	d = &I2CPorts[i2cCurr];

	i2cState[i2cCurr].addr = id;
	i2cState[i2cCurr].reg = reg;
	i2cState[i2cCurr].subaddress_sent = false;
	i2cState[i2cCurr].final_stop = false;
	i2cState[i2cCurr].writing = true;
	i2cState[i2cCurr].reading = false;
	i2cState[i2cCurr].write_p = my_data;
	i2cState[i2cCurr].read_p = my_data;
	i2cState[i2cCurr].bytes = len_;
	i2cState[i2cCurr].busy = true;

	for (i = 0; i < len_; i++)
		my_data[i] = data[i];

	if (!(d->I2C->CR2 & I2C_IT_EVT)) { //if we are restarting the driver
		if (!(d->I2C->CR1 & 0x0100)) { // ensure sending a start
			while (d->I2C->CR1 & 0x0200) { //wait for any stop to finish sending
			}
			I2C_GenerateSTART(d->I2C, ENABLE); //send the start for the new job
		}
		I2C_ITConfig(d->I2C, I2C_IT_EVT | I2C_IT_ERR, ENABLE); //allow the interrupts to fire off again
	}

	while (i2cState[i2cCurr].busy && --timeout > 0) {
	}
	if (timeout == 0) {
		i2cState[i2cCurr].i2cErrors++;
		NV.Stats[I2CFailS] = i2cState[i2cCurr].i2cErrors;
		i2cInit(i2cCurr);
		return (false);
	}

	return (true);
} // i2cWriteBlock


boolean i2cResponse(uint8 i2cSel, uint8 d) { // returns true unless there is an I2C timeout????
	uint8 v;

	v = 77;
	return (i2cReadBlock(i2cSel, d, 0, 1, &v) && (v != 77));

} // response

#endif
