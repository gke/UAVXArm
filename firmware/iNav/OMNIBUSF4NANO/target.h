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

#pragma once


#define TARGET_BOARD_IDENTIFIER "OBF4"

#define USBD_PRODUCT_STRING     "Omnibus F4"

#define DEFAULT_FEATURES        (FEATURE_VBAT | FEATURE_OSD)
#define SENSORS_SET (SENSOR_ACC)

#define LED0                    PB5

#define BEEPER                  PB4
#define BEEPER_INVERTED

#define USE_SPI

#define USE_SPI_DEVICE_1
#define USE_SPI_DEVICE_3

//#define USE_EXTI
//#define GYRO_INT_EXTI            PC4
//#define USE_MPU_DATA_READY_SIGNAL

#define MPU6000_CS_PIN          PA4
#define MPU6000_SPI_BUS         BUS_SPI1

#define USE_GYRO
#define USE_GYRO_MPU6000
#define GYRO_MPU6000_ALIGN      CW180_DEG

#define USE_ACC
#define USE_ACC_MPU6000
#define ACC_MPU6000_ALIGN       CW180_DEG

#define SPI3_NSS_PIN            PB3
#define SPI3_SCK_PIN            PC10
#define SPI3_MISO_PIN           PC11
#define SPI3_MOSI_PIN           PC12

#define USE_OSD
#define USE_MAX7456
#define MAX7456_SPI_BUS         BUS_SPI3
#define MAX7456_CS_PIN          PA15

#define USE_ADC
#define ADC_CHANNEL_1_PIN               PC1
#define ADC_CHANNEL_2_PIN               PC2
#define ADC_CHANNEL_3_PIN               PA0

#define CURRENT_METER_ADC_CHANNEL       ADC_CHN_1
#define VBAT_ADC_CHANNEL                ADC_CHN_2
#define RSSI_ADC_CHANNEL                ADC_CHN_3

#define USE_VCP
#define VBUS_SENSING_PIN        PC5
#define VBUS_SENSING_ENABLED

#define USE_UART1 //  // SBUS
#define UART1_RX_PIN            PA10
#define UART1_TX_PIN            PA9
#define UART1_AHB1_PERIPHERALS  RCC_AHB1Periph_DMA2

//#define USE_UART3
//#define UART3_RX_PIN            PB11
//#define UART3_TX_PIN            PB10

//#define USE_UART6
//#define UART6_RX_PIN            PC7
//#define UART6_TX_PIN            PC6

//#define USE_SOFTSERIAL1
//#define SOFTSERIAL_1_RX_PIN     NONE // PC8  //3
//-#define SOFTSERIAL_1_TX_PIN     NONE // PC9 // 0

#define SERIAL_PORT_COUNT       2 //VCP, USART1, USART3, USART6, SOFTSERIAL1

#define DEFAULT_RX_TYPE         RX_TYPE_SERIAL
#define DISABLE_RX_PWM_FEATURE

#define MAX_PWM_OUTPUT_PORTS    4
#define TARGET_MOTOR_COUNT      4

#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff
#define TARGET_IO_PORTD         0xffff

