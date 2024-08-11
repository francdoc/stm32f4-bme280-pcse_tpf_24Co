################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/Src/API_bme280.c \
../Drivers/API/Src/API_bme280_port.c \
../Drivers/API/Src/API_clock_date.c \
../Drivers/API/Src/API_delay.c \
../Drivers/API/Src/API_fsm.c \
../Drivers/API/Src/API_lcd.c \
../Drivers/API/Src/API_lcd_port.c \
../Drivers/API/Src/API_uart.c 

OBJS += \
./Drivers/API/Src/API_bme280.o \
./Drivers/API/Src/API_bme280_port.o \
./Drivers/API/Src/API_clock_date.o \
./Drivers/API/Src/API_delay.o \
./Drivers/API/Src/API_fsm.o \
./Drivers/API/Src/API_lcd.o \
./Drivers/API/Src/API_lcd_port.o \
./Drivers/API/Src/API_uart.o 

C_DEPS += \
./Drivers/API/Src/API_bme280.d \
./Drivers/API/Src/API_bme280_port.d \
./Drivers/API/Src/API_clock_date.d \
./Drivers/API/Src/API_delay.d \
./Drivers/API/Src/API_fsm.d \
./Drivers/API/Src/API_lcd.d \
./Drivers/API/Src/API_lcd_port.d \
./Drivers/API/Src/API_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/Src/%.o Drivers/API/Src/%.su Drivers/API/Src/%.cyclo: ../Drivers/API/Src/%.c Drivers/API/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API/Inc" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API/Src" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/BSP" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/BSP/STM32F4xx_Nucleo_144" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-Src

clean-Drivers-2f-API-2f-Src:
	-$(RM) ./Drivers/API/Src/API_bme280.cyclo ./Drivers/API/Src/API_bme280.d ./Drivers/API/Src/API_bme280.o ./Drivers/API/Src/API_bme280.su ./Drivers/API/Src/API_bme280_port.cyclo ./Drivers/API/Src/API_bme280_port.d ./Drivers/API/Src/API_bme280_port.o ./Drivers/API/Src/API_bme280_port.su ./Drivers/API/Src/API_clock_date.cyclo ./Drivers/API/Src/API_clock_date.d ./Drivers/API/Src/API_clock_date.o ./Drivers/API/Src/API_clock_date.su ./Drivers/API/Src/API_delay.cyclo ./Drivers/API/Src/API_delay.d ./Drivers/API/Src/API_delay.o ./Drivers/API/Src/API_delay.su ./Drivers/API/Src/API_fsm.cyclo ./Drivers/API/Src/API_fsm.d ./Drivers/API/Src/API_fsm.o ./Drivers/API/Src/API_fsm.su ./Drivers/API/Src/API_lcd.cyclo ./Drivers/API/Src/API_lcd.d ./Drivers/API/Src/API_lcd.o ./Drivers/API/Src/API_lcd.su ./Drivers/API/Src/API_lcd_port.cyclo ./Drivers/API/Src/API_lcd_port.d ./Drivers/API/Src/API_lcd_port.o ./Drivers/API/Src/API_lcd_port.su ./Drivers/API/Src/API_uart.cyclo ./Drivers/API/Src/API_uart.d ./Drivers/API/Src/API_uart.o ./Drivers/API/Src/API_uart.su

.PHONY: clean-Drivers-2f-API-2f-Src

