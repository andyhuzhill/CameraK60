################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../drivers/FatFs/diskio.c" \
"../drivers/FatFs/ff.c" 

C_SRCS += \
../drivers/FatFs/diskio.c \
../drivers/FatFs/ff.c 

OBJS += \
./drivers/FatFs/diskio.obj \
./drivers/FatFs/ff.obj 

OBJS_QUOTED += \
"./drivers/FatFs/diskio.obj" \
"./drivers/FatFs/ff.obj" 

C_DEPS += \
./drivers/FatFs/diskio.d \
./drivers/FatFs/ff.d 

C_DEPS_QUOTED += \
"./drivers/FatFs/diskio.d" \
"./drivers/FatFs/ff.d" 

OBJS_OS_FORMAT += \
./drivers/FatFs/diskio.obj \
./drivers/FatFs/ff.obj 


# Each subdirectory must supply rules for building sources it contributes
drivers/FatFs/diskio.obj: ../drivers/FatFs/diskio.c
	@echo 'Building file: $<'
	@echo 'Executing target #22 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/FatFs/diskio.args" -o "drivers/FatFs/diskio.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/FatFs/%.d: ../drivers/FatFs/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

drivers/FatFs/ff.obj: ../drivers/FatFs/ff.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/FatFs/ff.args" -o "drivers/FatFs/ff.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '


