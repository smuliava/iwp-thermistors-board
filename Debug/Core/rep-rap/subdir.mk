################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/rep-rap/RepRapFirmware.cpp 

OBJS += \
./Core/rep-rap/RepRapFirmware.o 

CPP_DEPS += \
./Core/rep-rap/RepRapFirmware.d 


# Each subdirectory must supply rules for building sources it contributes
Core/rep-rap/%.o Core/rep-rap/%.su Core/rep-rap/%.cyclo: ../Core/rep-rap/%.cpp Core/rep-rap/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-rep-2d-rap

clean-Core-2f-rep-2d-rap:
	-$(RM) ./Core/rep-rap/RepRapFirmware.cyclo ./Core/rep-rap/RepRapFirmware.d ./Core/rep-rap/RepRapFirmware.o ./Core/rep-rap/RepRapFirmware.su

.PHONY: clean-Core-2f-rep-2d-rap

