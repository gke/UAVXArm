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
#include "drivers/bus.h"

#ifdef UAVXF4V4

BUSDEV_REGISTER_SPI_TAG(busdev_mpu6000, DEVHW_MPU6000, BUS_SPI2, PB12, PC14, 0, DEVFLAGS_NONE);
BUSDEV_REGISTER_SPI_TAG(busdev_ms5611, DEVHW_MS5611, BUS_SPI2, PC5, NONE, 0, DEVFLAGS_USE_RAW_REGISTERS);
BUSDEV_REGISTER_SPI(busdev_hmc5983_spi, DEVHW_HMC5883, BUS_SPI2, PC4, NONE, DEVFLAGS_NONE);
BUSDEV_REGISTER_SPI(busdev_m25p16, DEVHW_M25P16, BUS_SPI2, PC3, NONE, DEVFLAGS_NONE);


#else

BUSDEV_REGISTER_I2C_TAG(busdev_mpu6050, DEVHW_MPU6050, BUS_I2C2, 0x68, NONE, 0, DEVFLAGS_NONE);
BUSDEV_REGISTER_I2C(busdev_ms5611, DEVHW_MS5611, BUS_I2C2, 0x77, NONE, DEVFLAGS_USE_RAW_REGISTERS);
BUSDEV_REGISTER_I2C_TAG(busdev_hmc5883, DEVHW_HMC5883, BUS_I2C2, 0x1E, NONE, 0, DEVFLAGS_NONE);
//BUSDEV_REGISTER_I2C(busdev_m25p16, DEVHW_M25P16,BUS_SPI2, 0xa0, NONE, DEVFLAGS_NONE);

#endif


const timerHardware_t timerHardware[] = {

		{ TIM2, IO_TAG(PA0), TIM_Channel_1, 0, IOCFG_AF_PP_PD, GPIO_AF_TIM2, TIM_USE_PPM }, // CPPM

		{ TIM4, IO_TAG(PB6), TIM_Channel_1, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 4/UAVX K1
		{ TIM4, IO_TAG(PB7), TIM_Channel_2, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 3/UAVX K2
		{ TIM4, IO_TAG(PB8), TIM_Channel_3, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 2/UAVX K3
		{ TIM4, IO_TAG(PB9), TIM_Channel_4, 1, IOCFG_AF_PP_PD, GPIO_AF_TIM4, TIM_USE_MC_MOTOR | TIM_USE_FW_SERVO }, // 1/UAVX K4
#ifdef UAVXF4V4
		{ TIM8, IO_TAG(PC6), TIM_Channel_1, 1, IOCFG_IPD, GPIO_AF_TIM8, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR  }, // UAVX K5
		{ TIM8, IO_TAG(PC7), TIM_Channel_2, 1, IOCFG_IPD, GPIO_AF_TIM8, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR }, // UAVX K6
#else
		{ TIM1, IO_TAG(PA11), TIM_Channel_4, 1, IOCFG_IPD, GPIO_AF_TIM1, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR  }, // UAVX K5
		{ TIM1, IO_TAG(PA8), TIM_Channel_1, 1, IOCFG_IPD, GPIO_AF_TIM1, TIM_USE_MC_MOTOR | TIM_USE_FW_MOTOR }, // UAVX K6
#endif
};


const int timerHardwareCount = sizeof(timerHardware) / sizeof(timerHardware[0]);

