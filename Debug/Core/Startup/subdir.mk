################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f429zitx.s 

OBJS += \
./Core/Startup/startup_stm32f429zitx.o 

S_DEPS += \
./Core/Startup/startup_stm32f429zitx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API/Inc" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/API/Src" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/BSP" -I"/home/franco/CESE/PDM/PdM_workspace/SPI_test/Drivers/BSP/STM32F4xx_Nucleo_144" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f429zitx.d ./Core/Startup/startup_stm32f429zitx.o

.PHONY: clean-Core-2f-Startup

