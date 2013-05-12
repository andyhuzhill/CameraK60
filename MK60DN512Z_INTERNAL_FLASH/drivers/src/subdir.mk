################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../drivers/src/ASCII.c" \
"../drivers/src/assert.c" \
"../drivers/src/bl144002.c" \
"../drivers/src/common.c" \
"../drivers/src/dma.c" \
"../drivers/src/ftm.c" \
"../drivers/src/i2c.c" \
"../drivers/src/io.c" \
"../drivers/src/k60_gpio.c" \
"../drivers/src/k60_sdhc.c" \
"../drivers/src/lcd.c" \
"../drivers/src/lptmr.c" \
"../drivers/src/nrf24l0.c" \
"../drivers/src/ov7725.c" \
"../drivers/src/pit.c" \
"../drivers/src/port.c" \
"../drivers/src/printf.c" \
"../drivers/src/sccb.c" \
"../drivers/src/spi.c" \
"../drivers/src/sysinit.c" \
"../drivers/src/uart.c" 

C_SRCS += \
../drivers/src/ASCII.c \
../drivers/src/assert.c \
../drivers/src/bl144002.c \
../drivers/src/common.c \
../drivers/src/dma.c \
../drivers/src/ftm.c \
../drivers/src/i2c.c \
../drivers/src/io.c \
../drivers/src/k60_gpio.c \
../drivers/src/k60_sdhc.c \
../drivers/src/lcd.c \
../drivers/src/lptmr.c \
../drivers/src/nrf24l0.c \
../drivers/src/ov7725.c \
../drivers/src/pit.c \
../drivers/src/port.c \
../drivers/src/printf.c \
../drivers/src/sccb.c \
../drivers/src/spi.c \
../drivers/src/sysinit.c \
../drivers/src/uart.c 

OBJS += \
./drivers/src/ASCII.obj \
./drivers/src/assert.obj \
./drivers/src/bl144002.obj \
./drivers/src/common.obj \
./drivers/src/dma.obj \
./drivers/src/ftm.obj \
./drivers/src/i2c.obj \
./drivers/src/io.obj \
./drivers/src/k60_gpio.obj \
./drivers/src/k60_sdhc.obj \
./drivers/src/lcd.obj \
./drivers/src/lptmr.obj \
./drivers/src/nrf24l0.obj \
./drivers/src/ov7725.obj \
./drivers/src/pit.obj \
./drivers/src/port.obj \
./drivers/src/printf.obj \
./drivers/src/sccb.obj \
./drivers/src/spi.obj \
./drivers/src/sysinit.obj \
./drivers/src/uart.obj 

OBJS_QUOTED += \
"./drivers/src/ASCII.obj" \
"./drivers/src/assert.obj" \
"./drivers/src/bl144002.obj" \
"./drivers/src/common.obj" \
"./drivers/src/dma.obj" \
"./drivers/src/ftm.obj" \
"./drivers/src/i2c.obj" \
"./drivers/src/io.obj" \
"./drivers/src/k60_gpio.obj" \
"./drivers/src/k60_sdhc.obj" \
"./drivers/src/lcd.obj" \
"./drivers/src/lptmr.obj" \
"./drivers/src/nrf24l0.obj" \
"./drivers/src/ov7725.obj" \
"./drivers/src/pit.obj" \
"./drivers/src/port.obj" \
"./drivers/src/printf.obj" \
"./drivers/src/sccb.obj" \
"./drivers/src/spi.obj" \
"./drivers/src/sysinit.obj" \
"./drivers/src/uart.obj" 

C_DEPS += \
./drivers/src/ASCII.d \
./drivers/src/assert.d \
./drivers/src/bl144002.d \
./drivers/src/common.d \
./drivers/src/dma.d \
./drivers/src/ftm.d \
./drivers/src/i2c.d \
./drivers/src/io.d \
./drivers/src/k60_gpio.d \
./drivers/src/k60_sdhc.d \
./drivers/src/lcd.d \
./drivers/src/lptmr.d \
./drivers/src/nrf24l0.d \
./drivers/src/ov7725.d \
./drivers/src/pit.d \
./drivers/src/port.d \
./drivers/src/printf.d \
./drivers/src/sccb.d \
./drivers/src/spi.d \
./drivers/src/sysinit.d \
./drivers/src/uart.d 

C_DEPS_QUOTED += \
"./drivers/src/ASCII.d" \
"./drivers/src/assert.d" \
"./drivers/src/bl144002.d" \
"./drivers/src/common.d" \
"./drivers/src/dma.d" \
"./drivers/src/ftm.d" \
"./drivers/src/i2c.d" \
"./drivers/src/io.d" \
"./drivers/src/k60_gpio.d" \
"./drivers/src/k60_sdhc.d" \
"./drivers/src/lcd.d" \
"./drivers/src/lptmr.d" \
"./drivers/src/nrf24l0.d" \
"./drivers/src/ov7725.d" \
"./drivers/src/pit.d" \
"./drivers/src/port.d" \
"./drivers/src/printf.d" \
"./drivers/src/sccb.d" \
"./drivers/src/spi.d" \
"./drivers/src/sysinit.d" \
"./drivers/src/uart.d" 

OBJS_OS_FORMAT += \
./drivers/src/ASCII.obj \
./drivers/src/assert.obj \
./drivers/src/bl144002.obj \
./drivers/src/common.obj \
./drivers/src/dma.obj \
./drivers/src/ftm.obj \
./drivers/src/i2c.obj \
./drivers/src/io.obj \
./drivers/src/k60_gpio.obj \
./drivers/src/k60_sdhc.obj \
./drivers/src/lcd.obj \
./drivers/src/lptmr.obj \
./drivers/src/nrf24l0.obj \
./drivers/src/ov7725.obj \
./drivers/src/pit.obj \
./drivers/src/port.obj \
./drivers/src/printf.obj \
./drivers/src/sccb.obj \
./drivers/src/spi.obj \
./drivers/src/sysinit.obj \
./drivers/src/uart.obj 


# Each subdirectory must supply rules for building sources it contributes
drivers/src/ASCII.obj: ../drivers/src/ASCII.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/ASCII.args" -o "drivers/src/ASCII.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/%.d: ../drivers/src/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '

drivers/src/assert.obj: ../drivers/src/assert.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/assert.args" -o "drivers/src/assert.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/bl144002.obj: ../drivers/src/bl144002.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/bl144002.args" -o "drivers/src/bl144002.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/common.obj: ../drivers/src/common.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/common.args" -o "drivers/src/common.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/dma.obj: ../drivers/src/dma.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/dma.args" -o "drivers/src/dma.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/ftm.obj: ../drivers/src/ftm.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/ftm.args" -o "drivers/src/ftm.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/i2c.obj: ../drivers/src/i2c.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/i2c.args" -o "drivers/src/i2c.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/io.obj: ../drivers/src/io.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/io.args" -o "drivers/src/io.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/k60_gpio.obj: ../drivers/src/k60_gpio.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/k60_gpio.args" -o "drivers/src/k60_gpio.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/k60_sdhc.obj: ../drivers/src/k60_sdhc.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/k60_sdhc.args" -o "drivers/src/k60_sdhc.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/lcd.obj: ../drivers/src/lcd.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/lcd.args" -o "drivers/src/lcd.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/lptmr.obj: ../drivers/src/lptmr.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/lptmr.args" -o "drivers/src/lptmr.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/nrf24l0.obj: ../drivers/src/nrf24l0.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/nrf24l0.args" -o "drivers/src/nrf24l0.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/ov7725.obj: ../drivers/src/ov7725.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/ov7725.args" -o "drivers/src/ov7725.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/pit.obj: ../drivers/src/pit.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/pit.args" -o "drivers/src/pit.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/port.obj: ../drivers/src/port.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/port.args" -o "drivers/src/port.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/printf.obj: ../drivers/src/printf.c
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/printf.args" -o "drivers/src/printf.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/sccb.obj: ../drivers/src/sccb.c
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/sccb.args" -o "drivers/src/sccb.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/spi.obj: ../drivers/src/spi.c
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/spi.args" -o "drivers/src/spi.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/sysinit.obj: ../drivers/src/sysinit.c
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/sysinit.args" -o "drivers/src/sysinit.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

drivers/src/uart.obj: ../drivers/src/uart.c
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"drivers/src/uart.args" -o "drivers/src/uart.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '


