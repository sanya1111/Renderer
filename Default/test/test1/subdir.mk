################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/test1/main.cpp 

OBJS += \
./test/test1/main.o 

CPP_DEPS += \
./test/test1/main.d 


# Each subdirectory must supply rules for building sources it contributes
test/test1/%.o: ../test/test1/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libdrm -I/usr/include/libkms -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


