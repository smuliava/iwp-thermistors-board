################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/rep-rap/heating/sensors/SensorWithPort.cpp \
../Core/rep-rap/heating/sensors/TemperatureSensor.cpp \
../Core/rep-rap/heating/sensors/Thermistor.cpp 

OBJS += \
./Core/rep-rap/heating/sensors/SensorWithPort.o \
./Core/rep-rap/heating/sensors/TemperatureSensor.o \
./Core/rep-rap/heating/sensors/Thermistor.o 

CPP_DEPS += \
./Core/rep-rap/heating/sensors/SensorWithPort.d \
./Core/rep-rap/heating/sensors/TemperatureSensor.d \
./Core/rep-rap/heating/sensors/Thermistor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/rep-rap/heating/sensors/%.o Core/rep-rap/heating/sensors/%.su Core/rep-rap/heating/sensors/%.cyclo: ../Core/rep-rap/heating/sensors/%.cpp Core/rep-rap/heating/sensors/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-rep-2d-rap-2f-heating-2f-sensors

clean-Core-2f-rep-2d-rap-2f-heating-2f-sensors:
	-$(RM) ./Core/rep-rap/heating/sensors/SensorWithPort.cyclo ./Core/rep-rap/heating/sensors/SensorWithPort.d ./Core/rep-rap/heating/sensors/SensorWithPort.o ./Core/rep-rap/heating/sensors/SensorWithPort.su ./Core/rep-rap/heating/sensors/TemperatureSensor.cyclo ./Core/rep-rap/heating/sensors/TemperatureSensor.d ./Core/rep-rap/heating/sensors/TemperatureSensor.o ./Core/rep-rap/heating/sensors/TemperatureSensor.su ./Core/rep-rap/heating/sensors/Thermistor.cyclo ./Core/rep-rap/heating/sensors/Thermistor.d ./Core/rep-rap/heating/sensors/Thermistor.o ./Core/rep-rap/heating/sensors/Thermistor.su

.PHONY: clean-Core-2f-rep-2d-rap-2f-heating-2f-sensors

