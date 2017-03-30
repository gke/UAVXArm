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

// Master Clock

volatile uint32 sysTickUptime = 0;
volatile uint32 sysTickCycleCounter = 0;

void SysTick_Handler(void) {
	sysTickCycleCounter = *DWT_CYCCNT;
	sysTickUptime++;
} // SysTick_Handler

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
		Catastrophe();
	}
}

void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
		Catastrophe();
	}
}

void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
		Catastrophe();
	}
}

void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
		Catastrophe();
	}
}

void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

//______________________________________________________________________________________________

// RC Timers

void TIM2_IRQHandler(void) {

	if (CurrComboPort1Config == CPPM_GPS_M7to10) {
		if ((TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) && (CurrComboPort1Config
				== CPPM_GPS_M7to10))
			RCSerialISR(TIM_GetCapture1(TIM2));
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	} else if (CurrComboPort1Config == ParallelPPM)
		RCParallelISR(TIM2);
} // TIM2_IRQHandler

void TIM3_IRQHandler(void) {

	if (CurrComboPort1Config == ParallelPPM)
		RCParallelISR(TIM3);
} // TIM3_IRQHandler


//______________________________________________________________________________________________

// DMA


#if defined(STM32F1)

void DMA1_Channel4_IRQHandler(void) {

	DMA_ClearITPendingBit(DMA1_IT_TC4);
	DMA_Cmd(DMA1_Channel4, DISABLE);

	TxQHead[0] = TxQNewHead[0];

	if (TxQHead[0] != TxQTail[0])
	serialTxDMA(0);
} // DMA1_Channel4_IRQHandler

void DMA1_Channel7_IRQHandler(void) {

	DMA_ClearITPendingBit(DMA1_IT_TC7);
	DMA_Cmd(DMA1_Channel7, DISABLE);

	TxQHead[1] = TxQNewHead[1];

	if (TxQHead[1] != TxQTail[1])
	serialTxDMA(1);
} // DMA1_Channel7_IRQHandler


#else

void DMA2_Stream2_IRQHandler(void) {

	// Half-Transfer completed
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_HTIF2)) {
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_HTIF2);
		wsUpdateBuffer(wsPWMBuffer);
	}

	// Transfer completed
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2)) {
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		wsUpdateBuffer(wsPWMBuffer + (PWM_BUFFER_SIZE / 2));
	}

} // DMA2_Stream2_IRQHandler


void DMA2_Stream7_IRQHandler(void) {

	DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
	DMA_Cmd(DMA2_Stream7, DISABLE);

	TxQHead[0] = TxQNewHead[0];
	if (TxQHead[0] != TxQTail[0])
		serialTxDMA(0);

} // DMA2_Channel7_IRQHandler

void DMA1_Stream6_IRQHandler(void) {

	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	DMA_Cmd(DMA1_Stream6, DISABLE);

	TxQHead[1] = TxQNewHead[1];
	if (TxQHead[1] != TxQTail[1])
		serialTxDMA(1);

} // DMA1_Channel6_IRQHandler

#if defined(V4_BOARD)

void DMA1_Stream3_IRQHandler(void) {

	DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
	DMA_Cmd(DMA1_Stream3, DISABLE);

	if (CurrComboPort2Config == GPS_RF_V4) {
		TxQHead[2] = TxQNewHead[2];
		if (TxQHead[2] != TxQTail[2])
			serialTxDMA(2);
	}

} // DMA1_Stream3_IRQHandler

#endif

#endif

//______________________________________________________________________________________________

// Serial

void USART1_IRQHandler(void) {
	serialISR(0);
} // USART1_IRQHandler

#if (MAX_SERIAL_PORTS > 1)

void USART2_IRQHandler(void) {
	serialISR(1);
} // USART2_IRQHandler

#if defined(V4_BOARD)
void USART3_IRQHandler(void) {
	if (CurrComboPort2Config == GPS_RF_V4)
		serialISR(2);
} // USART2_IRQHandler

#endif
#endif

//______________________________________________________________________________________________

// I2C


void I2C1_ER_IRQHandler(void) {
	i2c_er_handler(0);
}

void I2C1_EV_IRQHandler(void) {
	i2c_ev_handler(0);
}

void I2C2_ER_IRQHandler(void) {
	i2c_er_handler(1);
}

void I2C2_EV_IRQHandler(void) {
	i2c_ev_handler(1);
}

