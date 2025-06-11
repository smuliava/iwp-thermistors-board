################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/rep-rap/hard-ware/IoPorts.cpp 

OBJS += \
./Core/rep-rap/hard-ware/IoPorts.o 

CPP_DEPS += \
./Core/rep-rap/hard-ware/IoPorts.d 


# Each subdirectory must supply rules for building sources it contributes
Core/rep-rap/hard-ware/%.o Core/rep-rap/hard-ware/%.su Core/rep-rap/hard-ware/%.cyclo: ../Core/rep-rap/hard-ware/%.cpp Core/rep-rap/hard-ware/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-rep-2d-rap-2f-hard-2d-ware

clean-Core-2f-rep-2d-rap-2f-hard-2d-ware:
	-$(RM) ./Core/rep-rap/hard-ware/IoPorts.cyclo ./Core/rep-rap/hard-ware/IoPorts.d ./Core/rep-rap/hard-ware/IoPorts.o ./Core/rep-rap/hard-ware/IoPorts.su

.PHONY: clean-Core-2f-rep-2d-rap-2f-hard-2d-ware

