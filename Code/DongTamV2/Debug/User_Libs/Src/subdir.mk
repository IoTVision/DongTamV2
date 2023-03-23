################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User_Libs/Src/74HC595.c 

OBJS += \
./User_Libs/Src/74HC595.o 

C_DEPS += \
./User_Libs/Src/74HC595.d 


# Each subdirectory must supply rules for building sources it contributes
User_Libs/Src/%.o User_Libs/Src/%.su: ../User_Libs/Src/%.c User_Libs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../User_Libs/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User_Libs-2f-Src

clean-User_Libs-2f-Src:
	-$(RM) ./User_Libs/Src/74HC595.d ./User_Libs/Src/74HC595.o ./User_Libs/Src/74HC595.su

.PHONY: clean-User_Libs-2f-Src

