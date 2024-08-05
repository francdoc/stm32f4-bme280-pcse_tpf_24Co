################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/bme280.c 

OBJS += \
./Drivers/API/bme280.o 

C_DEPS += \
./Drivers/API/bme280.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/%.o Drivers/API/%.su Drivers/API/%.cyclo: ../Drivers/API/%.c Drivers/API/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API

clean-Drivers-2f-API:
	-$(RM) ./Drivers/API/bme280.cyclo ./Drivers/API/bme280.d ./Drivers/API/bme280.o ./Drivers/API/bme280.su

.PHONY: clean-Drivers-2f-API

