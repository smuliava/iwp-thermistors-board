################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/RepRapFirmware/Hardware/IoPorts.cpp 

OBJS += \
./Core/RepRapFirmware/Hardware/IoPorts.o 

CPP_DEPS += \
./Core/RepRapFirmware/Hardware/IoPorts.d 


# Each subdirectory must supply rules for building sources it contributes
Core/RepRapFirmware/Hardware/%.o Core/RepRapFirmware/Hardware/%.su Core/RepRapFirmware/Hardware/%.cyclo: ../Core/RepRapFirmware/Hardware/%.cpp Core/RepRapFirmware/Hardware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I/Core/rep-rap -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-RepRapFirmware-2f-Hardware

clean-Core-2f-RepRapFirmware-2f-Hardware:
	-$(RM) ./Core/RepRapFirmware/Hardware/IoPorts.cyclo ./Core/RepRapFirmware/Hardware/IoPorts.d ./Core/RepRapFirmware/Hardware/IoPorts.o ./Core/RepRapFirmware/Hardware/IoPorts.su

.PHONY: clean-Core-2f-RepRapFirmware-2f-Hardware

