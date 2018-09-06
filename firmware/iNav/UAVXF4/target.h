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

#define USE_TARGET_HARDWARE_DESCRIPTORS

#ifdef UAVXF4V4
#define TARGET_BOARD_IDENTIFIER "UAVX"
#define USBD_PRODUCT_STRING     "UAVXArmF4V4 Quadrocopter"
#else
#define TARGET_BOARD_IDENTIFIER "UAVX"
#define USBD_PRODUCT_STRING     "UAVXArmF4V3 Quadrocopter"
#endif

#define DISABLE_RX_PWM_FEATURE // do not use parallel Rx input

// Features

#define DEFAULT_FEATURES        (FEATURE_VBAT)

#define RX_CHANNELS_TAER

//#define USE_SERIAL_4WAY_BLHELI_INTERFACE

// Port Configs

#define MAX_PWM_OUTPUT_PORTS 	6
#define TARGET_MOTOR_COUNT      6
#define USE_SERVOS

#define TARGET_IO_PORTA         0xffff
#define TARGET_IO_PORTB         0xffff
#define TARGET_IO_PORTC         0xffff

#define YELLOW_LED			PB3
#define RED_LED				PB4
#define BLUE_LED			PC11
#define GREEN_LED			PC12

#define LED0                BLUE_LED
#define LED1                GREEN_LED
#define LED2				RED_LED
#define LED3				YELLOW_LED

#define LED0_INVERTED
#define LED1_INVERTED
#define LED2_INVERTED
#define LED3_INVERTED

//#define BEEPER_INVERTED

//#define LED_STRIP
// LED Strip can run off Pin 5 (PA1) of the MOTOR outputs.
//#define WS2811_GPIO_AF                  GPIO_AF_TIM8
//#define WS2811_PIN                      PC6
//#define WS2811_DMA_STREAM               DMA2_Stream2
//#define WS2811_DMA_CHANNEL              DMA_Channel_0
//#define WS2811_DMA_HANDLER_IDENTIFER    DMA2_ST2_HANDLER

//#define INVERTER_PIN_UART1      PC0 // PC0 used as inverter select GPIO


#define USE_ADC
#define ADC1_DMA_STREAM             DMA2_Stream0
#define ADC_CHANNEL_1_PIN       	PC0
#define ADC_CHANNEL_2_PIN       	PC1
#define ADC_CHANNEL_3_PIN       	PC2

#define RSSI_ADC_CHANNEL   			ADC_CHN_1
#define CURRENT_METER_ADC_CHANNEL   ADC_CHN_2
#define VBAT_ADC_CHANNEL            ADC_CHN_3

#define BOARD_HAS_VOLTAGE_DIVIDER

#define USE_UART1
#define UART1_RX_PIN            PA10
#define UART1_TX_PIN            PA9
#define UART1_AHB1_PERIPHERALS  RCC_AHB1Periph_DMA2

#define USE_UART2
#define UART2_RX_PIN            PA3
#define UART2_TX_PIN            PA2

#define SENSORS_SET (SENSOR_ACC|SENSOR_MAG|SENSOR_BARO|SENSOR_GPS)

#ifdef UAVXF4V4

#define USE_SPI
#define USE_SPI_DEVICE_2

#define SPI2_NSS_PIN            NONE
#define SPI2_SCK_PIN            PB13
#define SPI2_MISO_PIN           PB14
#define SPI2_MOSI_PIN           PB15

//#define USE_EXTI

#define USE_GYRO
#define USE_GYRO_MPU6000
#define GYRO_MPU6000_ALIGN      CW90_DEG

#define USE_ACC
#define USE_ACC_MPU6000
#define ACC_MPU6000_ALIGN       CW90_DEG

#define USE_BARO
#define USE_BARO_MS5611

#define USE_MAG
#define USE_MAG_HMC5883

//#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT
//#define USE_FLASHFS
//#define USE_FLASH_M25P16

#define USE_UART3
#define UART3_RX_PIN            PB11
#define UART3_TX_PIN            PB10

#define SERIAL_PORT_COUNT       3 //USART1, USART2, USART3

#define BEEPER                  PA4

#define GPS
#define GPS_UART				SERIAL_PORT_USART3

#define DEFAULT_RX_TYPE         RX_TYPE_SERIAL
#define SERIALRX_PROVIDER       SERIALRX_SBUS
#define SERIALRX_UART           SERIAL_PORT_USART2

#define TELEMETRY_UART          SERIAL_PORT_USART1

#else

#define USE_I2C
#define USE_I2C_DEVICE_2
#define I2C2_SCL                PB10
#define I2C2_SDA                PB11

#define USE_GYRO
#define USE_GYRO_MPU6050
#define GYRO_MPU6050_ALIGN      CW90_DEG

#define USE_ACC
#define USE_ACC_MPU6050
#define ACC_MPU6050_ALIGN       CW90_DEG

#define USE_BARO
#define USE_BARO_MS5611

#define USE_MAG
#define USE_MAG_HMC5883

//#define USE_FLASHFS
//#define USE_FLASH_M25P16
//#define ENABLE_BLACKBOX_LOGGING_ON_SPIFLASH_BY_DEFAULT

#define USE_SOFTSERIAL1
#define SOFTSERIAL_1_TX_PIN     PC7
#define SOFTSERIAL_1_RX_PIN     PC8

#define SERIAL_PORT_COUNT       3 //USART1, USART2, SOFTSERIAL1

#define BEEPER                  PA12

#define GPS
#define GPS_UART				SERIAL_PORT_USART2

#define DEFAULT_RX_TYPE         RX_TYPE_PPM

#define TELEMETRY_UART          SERIAL_PORT_SOFTSERIAL1

#endif


#define MAG_HMC5883_ALIGN       CW90_DEG

//#define USE_PITOT_ADC
//#define USE_PITOT_MS4525
//#define PITOT_I2C_INSTANCE      I2C_DEVICE

//#define MAG_GPS_ALIGN			CW180_DEG_FLIP

