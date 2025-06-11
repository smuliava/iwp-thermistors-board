################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/RepRapFirmware/Heating/Sensors/SensorWithPort.cpp \
../Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.cpp \
../Core/RepRapFirmware/Heating/Sensors/Thermistor.cpp 

OBJS += \
./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.o \
./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.o \
./Core/RepRapFirmware/Heating/Sensors/Thermistor.o 

CPP_DEPS += \
./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.d \
./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.d \
./Core/RepRapFirmware/Heating/Sensors/Thermistor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/RepRapFirmware/Heating/Sensors/%.o Core/RepRapFirmware/Heating/Sensors/%.su Core/RepRapFirmware/Heating/Sensors/%.cyclo: ../Core/RepRapFirmware/Heating/Sensors/%.cpp Core/RepRapFirmware/Heating/Sensors/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I/Core/rep-rap -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-RepRapFirmware-2f-Heating-2f-Sensors

clean-Core-2f-RepRapFirmware-2f-Heating-2f-Sensors:
	-$(RM) ./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.cyclo ./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.d ./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.o ./Core/RepRapFirmware/Heating/Sensors/SensorWithPort.su ./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.cyclo ./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.d ./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.o ./Core/RepRapFirmware/Heating/Sensors/TemperatureSensor.su ./Core/RepRapFirmware/Heating/Sensors/Thermistor.cyclo ./Core/RepRapFirmware/Heating/Sensors/Thermistor.d ./Core/RepRapFirmware/Heating/Sensors/Thermistor.o ./Core/RepRapFirmware/Heating/Sensors/Thermistor.su

.PHONY: clean-Core-2f-RepRapFirmware-2f-Heating-2f-Sensors

