################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/RepRapFirmware/ObjectModel/GlobalVariables.cpp \
../Core/RepRapFirmware/ObjectModel/ObjectModel.cpp \
../Core/RepRapFirmware/ObjectModel/Variable.cpp 

OBJS += \
./Core/RepRapFirmware/ObjectModel/GlobalVariables.o \
./Core/RepRapFirmware/ObjectModel/ObjectModel.o \
./Core/RepRapFirmware/ObjectModel/Variable.o 

CPP_DEPS += \
./Core/RepRapFirmware/ObjectModel/GlobalVariables.d \
./Core/RepRapFirmware/ObjectModel/ObjectModel.d \
./Core/RepRapFirmware/ObjectModel/Variable.d 


# Each subdirectory must supply rules for building sources it contributes
Core/RepRapFirmware/ObjectModel/%.o Core/RepRapFirmware/ObjectModel/%.su Core/RepRapFirmware/ObjectModel/%.cyclo: ../Core/RepRapFirmware/ObjectModel/%.cpp Core/RepRapFirmware/ObjectModel/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I/Core/rep-rap -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-RepRapFirmware-2f-ObjectModel

clean-Core-2f-RepRapFirmware-2f-ObjectModel:
	-$(RM) ./Core/RepRapFirmware/ObjectModel/GlobalVariables.cyclo ./Core/RepRapFirmware/ObjectModel/GlobalVariables.d ./Core/RepRapFirmware/ObjectModel/GlobalVariables.o ./Core/RepRapFirmware/ObjectModel/GlobalVariables.su ./Core/RepRapFirmware/ObjectModel/ObjectModel.cyclo ./Core/RepRapFirmware/ObjectModel/ObjectModel.d ./Core/RepRapFirmware/ObjectModel/ObjectModel.o ./Core/RepRapFirmware/ObjectModel/ObjectModel.su ./Core/RepRapFirmware/ObjectModel/Variable.cyclo ./Core/RepRapFirmware/ObjectModel/Variable.d ./Core/RepRapFirmware/ObjectModel/Variable.o ./Core/RepRapFirmware/ObjectModel/Variable.su

.PHONY: clean-Core-2f-RepRapFirmware-2f-ObjectModel

