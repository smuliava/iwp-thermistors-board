################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/RepRapFirmware/RepRapFirmware.cpp 

OBJS += \
./Core/RepRapFirmware/RepRapFirmware.o 

CPP_DEPS += \
./Core/RepRapFirmware/RepRapFirmware.d 


# Each subdirectory must supply rules for building sources it contributes
Core/RepRapFirmware/%.o Core/RepRapFirmware/%.su Core/RepRapFirmware/%.cyclo: ../Core/RepRapFirmware/%.cpp Core/RepRapFirmware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I/Core/rep-rap -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-RepRapFirmware

clean-Core-2f-RepRapFirmware:
	-$(RM) ./Core/RepRapFirmware/RepRapFirmware.cyclo ./Core/RepRapFirmware/RepRapFirmware.d ./Core/RepRapFirmware/RepRapFirmware.o ./Core/RepRapFirmware/RepRapFirmware.su

.PHONY: clean-Core-2f-RepRapFirmware

