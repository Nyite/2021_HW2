################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/Src/system_stm32f4xx.c 

OBJS += \
./CMSIS/Src/system_stm32f4xx.o 

C_DEPS += \
./CMSIS/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/Src/system_stm32f4xx.o: ../CMSIS/Src/system_stm32f4xx.c CMSIS/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VGTx -c -I"E:/ST/2021_HW2/CMSIS/Inc" -I"E:/ST/2021_HW2/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"CMSIS/Src/system_stm32f4xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

