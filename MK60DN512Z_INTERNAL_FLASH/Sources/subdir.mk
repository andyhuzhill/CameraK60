################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/controller.c" \
"../Sources/img_process.c" \
"../Sources/isr.c" \
"../Sources/main.c" 

C_SRCS += \
../Sources/controller.c \
../Sources/img_process.c \
../Sources/isr.c \
../Sources/main.c 

OBJS += \
./Sources/controller.obj \
./Sources/img_process.obj \
./Sources/isr.obj \
./Sources/main.obj 

OBJS_QUOTED += \
"./Sources/controller.obj" \
"./Sources/img_process.obj" \
"./Sources/isr.obj" \
"./Sources/main.obj" 

C_DEPS += \
./Sources/controller.d \
./Sources/img_process.d \
./Sources/isr.d \
./Sources/main.d 

C_DEPS_QUOTED += \
"./Sources/controller.d" \
"./Sources/img_process.d" \
"./Sources/isr.d" \
"./Sources/main.d" 

OBJS_OS_FORMAT += \
./Sources/controller.obj \
./Sources/img_process.obj \
./Sources/isr.obj \
./Sources/main.obj 


# Each subdirectory must supply rules for building sources it contributes
Sources/controller.obj: ../Sources/controller.c
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/controller.args" -o "Sources/controller.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/%.d: ../Sources/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

Sources/img_process.obj: ../Sources/img_process.c
	@echo 'Building file: $<'
	@echo 'Executing target #25 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/img_process.args" -o "Sources/img_process.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/isr.obj: ../Sources/isr.c
	@echo 'Building file: $<'
	@echo 'Executing target #26 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/isr.args" -o "Sources/isr.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.obj: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #27 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/main.args" -o "Sources/main.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '


