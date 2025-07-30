################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Errors/Errors.cpp 

OBJS += \
./Core/Src/Errors/Errors.o 

CPP_DEPS += \
./Core/Src/Errors/Errors.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Errors/%.o Core/Src/Errors/%.su Core/Src/Errors/%.cyclo: ../Core/Src/Errors/%.cpp Core/Src/Errors/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I"../Core/Src/Heating/Sensors" -I"../Core/Src/Heating" -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -mfp16-format=ieee -mfpu=neon-fp16 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Errors

clean-Core-2f-Src-2f-Errors:
	-$(RM) ./Core/Src/Errors/Errors.cyclo ./Core/Src/Errors/Errors.d ./Core/Src/Errors/Errors.o ./Core/Src/Errors/Errors.su

.PHONY: clean-Core-2f-Src-2f-Errors

