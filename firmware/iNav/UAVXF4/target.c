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
 */

#include <stdint.h>

#include <platform.h>
#include "drivers/io.h"

#include "drivers/dma.h"
#include "drivers/timer.h"

#include "drivers/pwm_mapping.h"

#if (FC_FIRMWARE_NAME == INAV)

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {

		{ TIM2, IO_TAG(PA0), TIM_Channel_1, 0, IOCFG_AF_PP_PD, GPIO_AF_TIM2, TIM_USE_PPM }, // CPPM

		{ TIM4, IO_TAG(PB6), TIM_Channel_1, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 4/UAVX K1
		{ TIM4, IO_TAG(PB7), TIM_Channel_2, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 3/UAVX K2
		{ TIM4, IO_TAG(PB8), TIM_Channel_3, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 2/UAVX K3
		{ TIM4, IO_TAG(PB9), TIM_Channel_4, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 1/UAVX K4

		{ TIM1, IO_TAG(PA11), TIM_Channel_4, 1, IOCFG_IPD, GPIO_AF_TIM1, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR  }, // UAVX K5
		{ TIM1, IO_TAG(PA8), TIM_Channel_1, 1, IOCFG_IPD, GPIO_AF_TIM1, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR }, // UAVX K6

};

#else

const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {

	DEF_TIM(TIM2, CH1, PA0, TIM_USE_PPM | TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // CPPM
	//DEF_TIM(TIM2, CH2, PA1, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM2 - RC2
	//DEF_TIM(TIM2, CH3, PA2, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM3 - RC3
	//DEF_TIM(TIM2, CH4, PA3, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM4 - RC4
	//DEF_TIM(TIM3, CH1, PA6, TIM_USE_PWM | TIM_USE_LED, TIMER_INPUT_ENABLED, 0), // PWM5 - RC5
	//DEF_TIM(TIM3, CH2, PA7, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM6 - RC6
	//DEF_TIM(TIM3, CH3, PB0, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM7 - RC7
	//DEF_TIM(TIM3, CH4, PB1, TIM_USE_PWM, TIMER_INPUT_ENABLED, 0), // PWM8 - RC8

	DEF_TIM(TIM4, CH1, PB6, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0) // 4/UAVX K1
	DEF_TIM(TIM4, CH2, PB7, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0), // 3/UAVX K2
	DEF_TIM(TIM4, CH3, PB8, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0), // 2/UAVX K3
	DEF_TIM(TIM4, CH4, PB9, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0), // 1/UAVX K4

	DEF_TIM(TIM1, CH4, PA11, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0), // UAVX K5
	DEF_TIM(TIM1, CH1, PA8, TIM_USE_MOTOR, TIMER_OUTPUT_ENABLED, 0), // UAVX K6

};
#endif


const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);

