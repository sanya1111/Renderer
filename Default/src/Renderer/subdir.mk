################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Renderer/smth.cpp 

OBJS += \
./src/Renderer/smth.o 

CPP_DEPS += \
./src/Renderer/smth.d 


# Each subdirectory must supply rules for building sources it contributes
src/Renderer/%.o: ../src/Renderer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libdrm -I/usr/include/libkms -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


