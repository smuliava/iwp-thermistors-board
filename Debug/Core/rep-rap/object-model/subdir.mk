################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/rep-rap/object-model/GlobalVariables.cpp \
../Core/rep-rap/object-model/ObjectModel.cpp \
../Core/rep-rap/object-model/Variable.cpp 

OBJS += \
./Core/rep-rap/object-model/GlobalVariables.o \
./Core/rep-rap/object-model/ObjectModel.o \
./Core/rep-rap/object-model/Variable.o 

CPP_DEPS += \
./Core/rep-rap/object-model/GlobalVariables.d \
./Core/rep-rap/object-model/ObjectModel.d \
./Core/rep-rap/object-model/Variable.d 


# Each subdirectory must supply rules for building sources it contributes
Core/rep-rap/object-model/%.o Core/rep-rap/object-model/%.su Core/rep-rap/object-model/%.cyclo: ../Core/rep-rap/object-model/%.cpp Core/rep-rap/object-model/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-rep-2d-rap-2f-object-2d-model

clean-Core-2f-rep-2d-rap-2f-object-2d-model:
	-$(RM) ./Core/rep-rap/object-model/GlobalVariables.cyclo ./Core/rep-rap/object-model/GlobalVariables.d ./Core/rep-rap/object-model/GlobalVariables.o ./Core/rep-rap/object-model/GlobalVariables.su ./Core/rep-rap/object-model/ObjectModel.cyclo ./Core/rep-rap/object-model/ObjectModel.d ./Core/rep-rap/object-model/ObjectModel.o ./Core/rep-rap/object-model/ObjectModel.su ./Core/rep-rap/object-model/Variable.cyclo ./Core/rep-rap/object-model/Variable.d ./Core/rep-rap/object-model/Variable.o ./Core/rep-rap/object-model/Variable.su

.PHONY: clean-Core-2f-rep-2d-rap-2f-object-2d-model

