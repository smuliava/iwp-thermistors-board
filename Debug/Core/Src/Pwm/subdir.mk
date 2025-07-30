################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Pwm/Pwm.cpp 

OBJS += \
./Core/Src/Pwm/Pwm.o 

CPP_DEPS += \
./Core/Src/Pwm/Pwm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Pwm/%.o Core/Src/Pwm/%.su Core/Src/Pwm/%.cyclo: ../Core/Src/Pwm/%.cpp Core/Src/Pwm/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I"../Core/Src/Heating/Sensors" -I"../Core/Src/Heating" -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -mfp16-format=ieee -mfpu=neon-fp16 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Pwm

clean-Core-2f-Src-2f-Pwm:
	-$(RM) ./Core/Src/Pwm/Pwm.cyclo ./Core/Src/Pwm/Pwm.d ./Core/Src/Pwm/Pwm.o ./Core/Src/Pwm/Pwm.su

.PHONY: clean-Core-2f-Src-2f-Pwm

