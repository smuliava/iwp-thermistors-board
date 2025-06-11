################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Heating/Sensors/Thermistor.cpp 

OBJS += \
./Core/Src/Heating/Sensors/Thermistor.o 

CPP_DEPS += \
./Core/Src/Heating/Sensors/Thermistor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Heating/Sensors/%.o Core/Src/Heating/Sensors/%.su Core/Src/Heating/Sensors/%.cyclo: ../Core/Src/Heating/Sensors/%.cpp Core/Src/Heating/Sensors/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I"D:/projects/STM32CubeIDE/workspace_1.18.0/iwp-thermistors-board/Core/Src/Heating/Sensors" -I"D:/projects/STM32CubeIDE/workspace_1.18.0/iwp-thermistors-board/Core/Src/Heating" -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Heating-2f-Sensors

clean-Core-2f-Src-2f-Heating-2f-Sensors:
	-$(RM) ./Core/Src/Heating/Sensors/Thermistor.cyclo ./Core/Src/Heating/Sensors/Thermistor.d ./Core/Src/Heating/Sensors/Thermistor.o ./Core/Src/Heating/Sensors/Thermistor.su

.PHONY: clean-Core-2f-Src-2f-Heating-2f-Sensors

