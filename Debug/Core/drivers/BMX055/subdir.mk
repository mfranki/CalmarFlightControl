################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/BMX055/BMX055.c 

OBJS += \
./Core/drivers/BMX055/BMX055.o 

C_DEPS += \
./Core/drivers/BMX055/BMX055.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/BMX055/BMX055.o: ../Core/drivers/BMX055/BMX055.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -D__VFP_FP__ -DSTM32F401xC -DDEBUG -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I"C:/Users/Michal/STM32CubeIDE/workspace/CalmarFlightController/Core" -I"C:/Users/Michal/STM32CubeIDE/workspace/CalmarFlightController/Core" -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/drivers/BMX055/BMX055.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

