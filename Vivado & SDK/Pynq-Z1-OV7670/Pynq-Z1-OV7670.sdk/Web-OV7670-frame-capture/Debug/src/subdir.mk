################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/dct.c \
../src/huffman_coding.c \
../src/image.c \
../src/jpg.c \
../src/main.c \
../src/ov7670_init.c \
../src/platform_zynq.c \
../src/server.c \
../src/vdma_single_step.c 

OBJS += \
./src/dct.o \
./src/huffman_coding.o \
./src/image.o \
./src/jpg.o \
./src/main.o \
./src/ov7670_init.o \
./src/platform_zynq.o \
./src/server.o \
./src/vdma_single_step.o 

C_DEPS += \
./src/dct.d \
./src/huffman_coding.d \
./src/image.d \
./src/jpg.d \
./src/main.d \
./src/ov7670_init.d \
./src/platform_zynq.d \
./src/server.d \
./src/vdma_single_step.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../OV7670-frame-capture_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


