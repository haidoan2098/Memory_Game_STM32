################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyLib/Src/BUZZER.c \
../MyLib/Src/Dou7SegLED.c \
../MyLib/Src/LCD_I2C.c 

OBJS += \
./MyLib/Src/BUZZER.o \
./MyLib/Src/Dou7SegLED.o \
./MyLib/Src/LCD_I2C.o 

C_DEPS += \
./MyLib/Src/BUZZER.d \
./MyLib/Src/Dou7SegLED.d \
./MyLib/Src/LCD_I2C.d 


# Each subdirectory must supply rules for building sources it contributes
MyLib/Src/%.o MyLib/Src/%.su MyLib/Src/%.cyclo: ../MyLib/Src/%.c MyLib/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../MyLib/Inc -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MyLib-2f-Src

clean-MyLib-2f-Src:
	-$(RM) ./MyLib/Src/BUZZER.cyclo ./MyLib/Src/BUZZER.d ./MyLib/Src/BUZZER.o ./MyLib/Src/BUZZER.su ./MyLib/Src/Dou7SegLED.cyclo ./MyLib/Src/Dou7SegLED.d ./MyLib/Src/Dou7SegLED.o ./MyLib/Src/Dou7SegLED.su ./MyLib/Src/LCD_I2C.cyclo ./MyLib/Src/LCD_I2C.d ./MyLib/Src/LCD_I2C.o ./MyLib/Src/LCD_I2C.su

.PHONY: clean-MyLib-2f-Src

