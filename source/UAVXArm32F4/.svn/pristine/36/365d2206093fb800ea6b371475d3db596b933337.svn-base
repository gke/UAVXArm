// DO NOT FORMAT  DO NOT FORMAT DO NOT FORMAT DO NOT FORMAT DO NOT FORMAT DO NOT FORMAT DO NOT FORMAT
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


#include "../UAVX.h"

PinDef RCPins[MAX_RC_INPS] = {
    { GPIOA, GPIO_Pin_0, GPIO_PinSource0, GPIO_Mode_IPU, 0, 0,
        true, {TIM2, TIM_Channel_1, TIM_IT_CC1, 0, 0 }, TIM2_IRQn},
    { GPIOA, GPIO_Pin_1, GPIO_PinSource1, GPIO_Mode_IPU, 0, 0,
        true, {TIM2, TIM_Channel_2, TIM_IT_CC2, 0, 0 }, TIM2_IRQn},
    { GPIOA, GPIO_Pin_2, GPIO_PinSource2, GPIO_Mode_IPU, 0, 0,
       	true, {TIM2, TIM_Channel_3, TIM_IT_CC3, 0, 0 }, TIM2_IRQn},
    { GPIOA, GPIO_Pin_3, GPIO_PinSource3, GPIO_Mode_IPU, 0, 0,
        true, {TIM2, TIM_Channel_4, TIM_IT_CC4, 0, 0 }, TIM2_IRQn},
    { GPIOA, GPIO_Pin_6, GPIO_PinSource6, GPIO_Mode_IPU, 0, 0,
        true, {TIM3, TIM_Channel_1, TIM_IT_CC1, 0, 0 }, TIM3_IRQn},
    { GPIOA, GPIO_Pin_7, GPIO_PinSource7, GPIO_Mode_IPU, 0, 0,
        true, {TIM3, TIM_Channel_2, TIM_IT_CC2, 0, 0 }, TIM3_IRQn},
    { GPIOB, GPIO_Pin_0, GPIO_PinSource0, GPIO_Mode_IPU, 0, 0,
        true, {TIM3, TIM_Channel_3, TIM_IT_CC3, 0, 0 }, TIM3_IRQn},
    { GPIOB, GPIO_Pin_1, GPIO_PinSource1, GPIO_Mode_IPU, 0, 0,
        true, {TIM3, TIM_Channel_4, TIM_IT_CC4, 0, 0 }, TIM3_IRQn}
        };

I2CPortDef I2CPorts[MAX_I2C_PORTS] = {
    { false, I2C1, 1,
    	GPIOB, GPIO_Pin_6, GPIO_PinSource6,
    	GPIOB, GPIO_Pin_7, GPIO_PinSource7,
    	0},
    { true, I2C2, 2,
    	GPIOB, GPIO_Pin_10, GPIO_PinSource10,
    	GPIOB, GPIO_Pin_11, GPIO_PinSource11,
    	0 }
    };

AnalogPinDef AnalogPins[ANALOG_CHANNELS] = {
    { ADC1, GPIOC, GPIO_Pin_0, ADC_Channel_10, 0, DMA1_Channel1, 1}, // RF
    { ADC1, GPIOC, GPIO_Pin_1, ADC_Channel_11, 0, DMA1_Channel1, 2}, // Amps
    { ADC1, GPIOC, GPIO_Pin_2, ADC_Channel_12, 0, DMA1_Channel1, 3}, // Volts
    //TODO: clip analog channels if digitial gyro and/or only super sample gyros
    { ADC1, GPIOC, GPIO_Pin_3, ADC_Channel_13, 0, DMA1_Channel1, 4}, // Roll
    { ADC1, GPIOC, GPIO_Pin_4, ADC_Channel_14, 0, DMA1_Channel1, 5}, // Pitch
    { ADC1, GPIOC, GPIO_Pin_5, ADC_Channel_15, 0, DMA1_Channel1, 6},  // Yaw
    };

PinDef PWMPins[MAX_PWM_OUTPUTS] = {
	// Drives 1-4
    { GPIOB, GPIO_Pin_9, GPIO_PinSource9, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM4, TIM_Channel_4, 0, &(TIM4->CCR4), 0}},
    { GPIOB, GPIO_Pin_8, GPIO_PinSource8, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM4, TIM_Channel_3, 0, &(TIM4->CCR3), 0}},
    { GPIOB, GPIO_Pin_7, GPIO_PinSource7, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM4, TIM_Channel_2, 0, &(TIM4->CCR2), 0}},
    { GPIOB, GPIO_Pin_6, GPIO_PinSource6, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM4, TIM_Channel_1, 0, &(TIM4->CCR1), 0}},
    { GPIOA, GPIO_Pin_11, GPIO_PinSource11, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM1, TIM_Channel_4, 0, &(TIM1->CCR4), 0}},
    { GPIOA, GPIO_Pin_8, GPIO_PinSource8, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM1, TIM_Channel_1, 0, &(TIM1->CCR1), 0}},
    // Drives 4-8 for brushless
    { GPIOA, GPIO_Pin_6, GPIO_PinSource6, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM3, TIM_Channel_1, 0, &(TIM3->CCR1) , 0}},
    { GPIOA, GPIO_Pin_7, GPIO_PinSource7, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM3, TIM_Channel_2, 0, &(TIM3->CCR2), 0}},
    { GPIOB, GPIO_Pin_0, GPIO_PinSource0, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM3, TIM_Channel_3, 0, &(TIM3->CCR3), 0}},
    { GPIOB, GPIO_Pin_1, GPIO_PinSource1, GPIO_Mode_AF_PP, 0, 0,
        true, {TIM3, TIM_Channel_4, 0, &(TIM3->CCR4), 0}}
    };

PinDef GPIOPins[MAX_GPIO_PINS] = {
    { GPIOA, GPIO_Pin_12, GPIO_PinSource12, GPIO_Mode_Out_PP, 0, 0,
        false, { 0, }, 0, }, // Beeper
    { GPIOC, GPIO_Pin_10, GPIO_PinSource10, GPIO_Mode_IPU, 0, 0,
        false, { 0, }, 0, }, // Armed
    { GPIOC, GPIO_Pin_9, GPIO_PinSource9, GPIO_Mode_IPU, 0, 0,
        false, { 0, }, 0, }, // Landing

    { GPIOC, GPIO_Pin_6, GPIO_PinSource6, GPIO_Mode_IPU, 0, 0,
        false, { 0, }, 0, }, // Aux1
    { GPIOC, GPIO_Pin_7, GPIO_PinSource7, GPIO_Mode_IPU, 0, 0,
        false, { 0, }, 0, }, // Aux2
    { GPIOC, GPIO_Pin_8, GPIO_PinSource8, GPIO_Mode_Out_PP, 0, 0,
        false, { 0, }, 0, }, // Aux3 / Probe
    };

SPIPortDef SPIPorts[MAX_SPI_PORTS] = {
	{ SPI1, GPIOA,  {{GPIO_Pin_5, GPIO_PinSource5},
			{GPIO_Pin_6, GPIO_PinSource6},
			{GPIO_Pin_7, GPIO_PinSource7}},
			false},
	{ SPI2, GPIOB, {{GPIO_Pin_13, GPIO_PinSource13},
			{GPIO_Pin_14, GPIO_PinSource14},
			{GPIO_Pin_15, GPIO_PinSource15}},
			true},
	{ SPI3, GPIOC, {{GPIO_Pin_10, GPIO_PinSource10},
			{GPIO_Pin_11, GPIO_PinSource11},
			{GPIO_Pin_12, GPIO_PinSource12}},
			false}
	};


PinDef SPISelectPins[MAX_SPI_DEVICES] = {
	{ GPIOB, GPIO_Pin_12, GPIO_PinSource12, GPIO_Mode_Out_PP, 0, 0,
		false, { 0, }, 0, }, // LISL for UAVP adapter
};



PinDef LEDPins[MAX_LEDS] = {
	{ GPIOB, GPIO_Pin_3, GPIO_PinSource3, GPIO_Mode_Out_OD, 0, 0,
        false, { 0, }, 0, }, // LED0
    { GPIOB, GPIO_Pin_4, GPIO_PinSource4, GPIO_Mode_Out_OD, 0, 0,
        false, { 0, }, 0, }, // LED1
    { GPIOC, GPIO_Pin_11, GPIO_PinSource11, GPIO_Mode_Out_OD, 0, 0,
        false, { 0, }, 0, }, // LED2
    { GPIOC, GPIO_Pin_12, GPIO_PinSource12, GPIO_Mode_Out_OD, 0, 0,
        false, { 0, }, 0, }, // LED3
        };

SerialPortDef SerialPorts[MAX_SERIAL_PORTS] = { // Tx, Rx
    { USART1, 0, GPIOA,
    	GPIO_Pin_9, GPIO_PinSource9,
    	GPIO_Pin_10, GPIO_PinSource10,
    	true, USART1_IRQn,
        true, 0,
        DMA1_Channel4, DMA1_Channel4_IRQn,
        DMA1_Channel5,
        115200 },
    { USART2, 1, GPIOA,
    	GPIO_Pin_2, GPIO_PinSource2,
    	GPIO_Pin_3, GPIO_PinSource3,
    	true, USART2_IRQn,
        false, 0,
        DMA1_Channel7, DMA1_Channel7_IRQn,
        DMA1_Channel6,
        115200 }
    };



