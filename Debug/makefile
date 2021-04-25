################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/middleware/rollingBuffer/subdir.mk
-include Core/middleware/posCalc/subdir.mk
-include Core/drivers/utils/subdir.mk
-include Core/drivers/uart/subdir.mk
-include Core/drivers/radio/subdir.mk
-include Core/drivers/BMX055/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
CalmarFlightController.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
CalmarFlightController.list \

OBJCOPY_BIN += \
CalmarFlightController.bin \


# All Target
all: CalmarFlightController.elf secondary-outputs

# Tool invocations
CalmarFlightController.elf: $(OBJS) $(USER_OBJS) C:\Users\Michal\STM32CubeIDE\workspace\CalmarFlightController\STM32F401RBTX_FLASH.ld
	arm-none-eabi-gcc -o "CalmarFlightController.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Michal\STM32CubeIDE\workspace\CalmarFlightController\STM32F401RBTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="CalmarFlightController.map" -Wl,--gc-sections -static -u _printf_float  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

CalmarFlightController.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "CalmarFlightController.list"
	@echo 'Finished building: $@'
	@echo ' '

CalmarFlightController.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "CalmarFlightController.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
