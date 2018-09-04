#

FEATURES       += ONBOARDFLASH

TARGET_SRC = \
            drivers/accgyro/accgyro_mpu6050.c \
            drivers/barometer/barometer_ms56xx.c \
            drivers/compass/compass_hmc5883l.c \
            drivers/light_ws2811strip.c \
            drivers/light_ws2811strip_stdperiph.c \
            drivers/serial_usb_vcp.c
            